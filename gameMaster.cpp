#include <sys/unistd.h>
#include <assert.h>
#include "gameMaster.h"

color gameMaster::en_posicion(coordenadas coord) {
	return tablero[coord.first][coord.second];
}

int gameMaster::getTamx() {
	return x;
}

int gameMaster::getTamy() {
	return y;
}

int gameMaster::distancia(coordenadas c1, coordenadas c2) {
    return abs(c1.first-c2.first)+abs(c1.second-c2.second);
}

gameMaster::gameMaster(Config config) {
	assert(config.x>0); 
	assert(config.y>0); // Tamaño adecuado del tablero

    this->x = config.x;
	this->y = config.y;

	assert((config.bandera_roja.first == 1)); // Bandera roja en la primera columna
	assert(es_posicion_valida(config.bandera_roja)); // Bandera roja en algún lugar razonable

	assert((config.bandera_azul.first == x-1)); // Bandera azul en la primera columna
	assert(es_posicion_valida(config.bandera_azul)); // Bandera roja en algún lugar razonable

	assert(config.pos_rojo.size() == config.cantidad_jugadores);
	assert(config.pos_azul.size() == config.cantidad_jugadores);
	for(auto &coord : config.pos_rojo) {
		assert(es_posicion_valida(coord)); // Posiciones validas rojas
	}		

	for(auto &coord : config.pos_azul) {
		assert(es_posicion_valida(coord)); // Posiciones validas rojas
	}		

	
	this->jugadores_por_equipos = config.cantidad_jugadores;
	this->pos_bandera_roja = config.bandera_roja;
	this->pos_bandera_azul = config.bandera_azul;
    this->pos_jugadores_rojos = config.pos_rojo;
    this->pos_jugadores_azules = config.pos_azul;
	// Seteo tablero
	tablero.resize(x);
    for (int i = 0; i < x; ++i) {
        tablero[i].resize(y);
        fill(tablero[i].begin(), tablero[i].end(), VACIO);
    }
    

    for(auto &coord : config.pos_rojo){
        assert(es_color_libre(tablero[coord.first][coord.second])); //Compruebo que no haya otro jugador en esa posicion
        tablero[coord.first][coord.second] = ROJO; // guardo la posicion
    }

    for(auto &coord : config.pos_azul){
        assert(es_color_libre(tablero[coord.first][coord.second]));
        tablero[coord.first][coord.second] = AZUL;
    }

    tablero[config.bandera_roja.first][config.bandera_roja.second] = BANDERA_ROJA;
    tablero[config.bandera_azul.first][config.bandera_azul.second] = BANDERA_AZUL;
	this->turno = ROJO;

	assert(sem_init(&this->turno_azul, 0, 0) == 0);
	assert(sem_init(&this->turno_rojo, 0, 0) == 0);
    assert(sem_init(&this->barrier, 0, 1) == 0);

    cout << "SE HA INICIALIZADO GAMEMASTER CON EXITO" << endl;
    // Insertar código que crea necesario de inicialización 
}

bool gameMaster::es_posicion_valida(coordenadas pos) {
	return (pos.first > 0) && (pos.first < x) && (pos.second > 0) && (pos.second < y);
}

bool gameMaster::es_color_libre(color color_tablero){
	bool bandera;
	if(turno == ROJO){
		bandera = (color_tablero == BANDERA_AZUL);
	}
	else{
		bandera = (color_tablero == BANDERA_ROJA);
	}
    return color_tablero == VACIO || color_tablero == INDEFINIDO || bandera;
}


bool gameMaster::se_puede_mover(coordenadas pos_actual, direccion dir){
	 coordenadas pos_nueva = proxima_posicion(pos_actual, dir);
	 return es_posicion_valida(pos_nueva) && es_color_libre(tablero[pos_nueva.first][pos_nueva.second]);
}

void gameMaster::mover_jugador_tablero(coordenadas pos_anterior, coordenadas pos_nueva, color colorEquipo){
    assert(es_color_libre(tablero[pos_nueva.first][pos_nueva.second]));
    tablero[pos_anterior.first][pos_anterior.second] = VACIO; 
    tablero[pos_nueva.first][pos_nueva.second] = colorEquipo;
}


int gameMaster::mover_jugador(direccion dir, int nro_jugador) {
	// Chequear que la movida sea valida. Que no se puedan mover 2 jugadores a la vez
    // setear la variable ganador. Devolver acorde a la descripción
	coordenadas pos_actual, prox_pos;
	this->turno == ROJO ? pos_actual = this->pos_jugadores_rojos[nro_jugador] : pos_actual = this->pos_jugadores_azules[nro_jugador];
    prox_pos = this->proxima_posicion(pos_actual, dir);
    assert(es_posicion_valida(prox_pos));
    mover_jugador_tablero(pos_actual, prox_pos, this->turno);
    (turno == ROJO ? pos_jugadores_rojos : pos_jugadores_azules)[nro_jugador] = prox_pos;
	coordenadas bandera_contraria = (turno == ROJO ? pos_bandera_azul : pos_bandera_roja);
    if(prox_pos == bandera_contraria){
        cout << "END GAME" << endl;
        this->ganador = this->turno;
    }
    return 	(this->termino_juego() ? 0 : nro_ronda);
}


void gameMaster::termino_ronda(color equipo) {
	// FIXME: Hacer chequeo de que es el color correcto que está llamando
	// FIXME: Hacer chequeo que hayan terminado todos los jugadores del equipo o su quantum (via mover_jugador)
    this->dibujame();
	assert(this->turno == equipo || this->termino_juego());
    this->nro_ronda++;
    this->turno = (equipo == ROJO) ? AZUL : ROJO;
	if(this->termino_juego() || this->nro_ronda > 100){
        if(this->nro_ronda > 100){
            this->ganador = EMPATE;
        }
		for(int i=0; i<10*this->jugadores_por_equipos; i++){
			sem_post(&this->turno_rojo);
            sem_post(&this->turno_azul);
		}
	}
	else{
        for(int i=0; i<this->jugadores_por_equipos; i++){
            sem_wait(&this->barrier);
        }
		for(int i=0; i<this->jugadores_por_equipos;i++){
			this->turno == ROJO ? sem_post(&this->turno_rojo) : sem_post(&this->turno_azul);
		}
	}
}

bool gameMaster::termino_juego() {
	return ganador != INDEFINIDO;
}

coordenadas gameMaster::proxima_posicion(coordenadas anterior, direccion movimiento) {
	// Calcula la proxima posición a moverse (es una copia) 
	switch(movimiento) {
        case(ARRIBA):
            anterior.first--;
            break;
        case(ABAJO):
            anterior.first++;
            break;
        case(IZQUIERDA):
            anterior.second--;
            break;
        case(DERECHA):
            anterior.second++;
            break;
    }
    return anterior; 
}

coordenadas gameMaster::pos_bandera(color equipo) {
    return (equipo == ROJO) ? pos_bandera_roja : pos_bandera_azul;
}

color gameMaster::equipo_jugando() {
    return this->turno;
}

void gameMaster::dibujame(){
    for(int i=0; i<x; i++){
        for(int j=0; j<y; j++){
            if(tablero[j][i] == 0){
                cout << "b" << " ";
            }else if(tablero[j][i] == 1){
                cout << "r" << " ";
            }else if(tablero[j][i] == 2){
                cout << "?" << " ";
            }else if(tablero[j][i] == 3){
                cout << "." << " ";
            }else if(tablero[j][i] == 4){
                cout << "?" << " ";
            }else if(tablero[j][i] == 5){
                cout << "R" << " ";
            }else if(tablero[j][i] == 6){
                cout << "B" << " ";
            }
        }
        cout << endl;
    }
    for(int i=0; i<x; i++){
        cout << "--";
    }
    cout << '\n';
}

color gameMaster::dame_equipo_actual() {
    return this->turno;
} 

int gameMaster::get_nro_ronda() {
    return this->nro_ronda;
}
