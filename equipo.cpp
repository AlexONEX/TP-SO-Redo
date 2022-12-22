#include "equipo.h"
#include <assert.h>
#include <limits>

mutex mt;

direccion Equipo::apuntar_a(coordenadas pos1, coordenadas pos2) {
    if (pos2.first > pos1.first)   { return ABAJO; }
    if (pos2.first < pos1.first)   { return ARRIBA; }
    if (pos2.second > pos1.second) { return DERECHA; }
    if (pos2.second < pos1.second) { return IZQUIERDA; }
}


Equipo::Equipo(gameMaster *belcebu, color equipo, 
		estrategia strat, int cant_jugadores, int quantum, vector<coordenadas> posiciones) {
	this->belcebu = belcebu;
	this->equipo = equipo;
	this->contrario = (equipo == ROJO)? AZUL: ROJO;
	this->bandera_contraria = (equipo==ROJO)? BANDERA_AZUL: BANDERA_ROJA;
	this->strat = strat;
	this->quantum = quantum;
	this->quantum_restante = quantum;
	this->cant_jugadores = cant_jugadores;
	this->posiciones = posiciones;
	//
	// ...
	//
	this->cant_jugadores_ya_jugaron = 0;
	assert(this->posiciones.size() == this->cant_jugadores);
	
	this->pos_bandera_contraria = this->belcebu->pos_bandera(this->contrario);
	this->jugador_min_distancia  = this->jugador_minima_distancia();
	this->jugador_max_distancia  = this->jugador_maxima_distancia();

	this->vec_sem = vector<sem_t>(this->cant_jugadores);
	for (int i = 0; i < this->cant_jugadores; i++) {
        assert(sem_init(&this->vec_sem[i], 0, 0) == 0);
    }
	assert(sem_init(&this->barrier, 0, 0) == 0);
}


void Equipo::jugador(int nro_jugador) {
	//
	// ...
	//

	while(!this->belcebu->termino_juego()) { // Chequear que no haya una race condition en gameMaster
		// Espero turno
		// Si soy rojo espero mi semaforo. Idem si azul.
		if(this->equipo == ROJO) {
			sem_wait(&this->belcebu->turno_rojo);
		} else {
			sem_wait(&this->belcebu->turno_azul);
		}
		if(this->belcebu->termino_juego()) {
			return;
		}
		assert(this->equipo == this->belcebu->equipo_jugando());
		//cout << "J IN " << nro_jugador << " " << this->equipo << endl;
        coordenadas pos_actual = this->posiciones[nro_jugador];
        direccion dir = apuntar_a(pos_actual, this->pos_bandera_contraria);
		switch(this->strat) {
			//SECUENCIAL,RR,SHORTEST,USTEDES
			case(SECUENCIAL):
				mt.lock();
				if(this->belcebu->se_puede_mover(pos_actual, dir) && this->belcebu->termino_juego() == false) {
					this->belcebu->mover_jugador(dir, nro_jugador);
					this->posiciones[nro_jugador] = this->belcebu->proxima_posicion(pos_actual, dir);
				}
				this->cant_jugadores_ya_jugaron++;
				if(this->cant_jugadores_ya_jugaron == this->cant_jugadores || this->belcebu->termino_juego()) {
					this->cant_jugadores_ya_jugaron = 0;
					for(int i=0; i<this->cant_jugadores; i++) {
						sem_post(&this->barrier);
					}
					this->belcebu->termino_ronda(this->equipo);
				}
				mt.unlock();
				sem_wait(&this->barrier);
				sem_post(&this->belcebu->barrier);
				break;
			case(RR):
				//
				// ...
				//
				break;

			case(SHORTEST):
				//
				// ...
				//
				mt.lock();
				if(nro_jugador==this->jugador_min_distancia && this->belcebu->se_puede_mover(pos_actual, dir)) {
					this->belcebu->mover_jugador(dir, nro_jugador);
					this->posiciones[nro_jugador] = this->belcebu->proxima_posicion(pos_actual, dir);
					this->jugador_min_distancia = this->jugador_minima_distancia();
				}
				this->cant_jugadores_ya_jugaron++;
				if(this->cant_jugadores_ya_jugaron == this->cant_jugadores || this->belcebu->termino_juego()) {
					this->cant_jugadores_ya_jugaron = 0;
					for(int i=0; i<this->cant_jugadores; i++) {
						sem_post(&this->barrier);
					}
					this->belcebu->termino_ronda(this->equipo);
				}
				mt.unlock();
				sem_wait(&this->barrier);
				sem_post(&this->belcebu->barrier);
				break;

			case(USTEDES):
				//
				// ...
				//
				mt.lock();
				if(nro_jugador==this->jugador_max_distancia && this->belcebu->se_puede_mover(pos_actual, dir)) {
					this->belcebu->mover_jugador(dir, nro_jugador);
					this->posiciones[nro_jugador] = this->belcebu->proxima_posicion(pos_actual, dir);
					this->jugador_max_distancia = this->jugador_maxima_distancia();
				}
				this->cant_jugadores_ya_jugaron++;
				if(this->cant_jugadores_ya_jugaron == this->cant_jugadores || this->belcebu->termino_juego()) {
					this->cant_jugadores_ya_jugaron = 0;
					for(int i=0; i<this->cant_jugadores; i++) {
						sem_post(&this->barrier);
					}
					this->belcebu->termino_ronda(this->equipo);
				}
				mt.unlock();
				sem_wait(&this->barrier);
				sem_post(&this->belcebu->barrier);
				break;
				break;
				break;
			default:
				break;
		}	
		// Termino ronda ? Recordar llamar a belcebu...
		// OJO. Esto lo termina un jugador... 
		//
		// ...
		//
	}
	return;
}


void Equipo::comenzar() {
	// Arranco cuando me toque el turno 
	// TODO: Quien empieza ? 
	//
	// ...
	//

	if (this->equipo == ROJO) {
        for (int i = 0; i < this->cant_jugadores; i++) {
            sem_post(&this->belcebu->turno_rojo);
        }
    }
    if(this->strat==RR){
        sem_post(&this->vec_sem[0]);
    }
	else if(this->strat==SHORTEST){
		this->jugador_min_distancia = this->jugador_minima_distancia();
		sem_post(&this->vec_sem[this->jugador_min_distancia]);
	}
	else if(this->strat==USTEDES){
		this->jugador_max_distancia = this->jugador_maxima_distancia();
		sem_post(&this->vec_sem[this->jugador_max_distancia]);
	}
	if(this->strat==USTEDES){
		this->jugador_max_distancia = this->jugador_maxima_distancia();
		sem_post(&this->vec_sem[this->jugador_max_distancia]);
	}

	// Creamos los jugadores
	for(int i=0; i < cant_jugadores; i++) {
		jugadores.emplace_back(thread(&Equipo::jugador, this, i)); 
	}
}

void Equipo::terminar() {
	for(auto &t:jugadores){
		t.join();
	}	
}

coordenadas Equipo::buscar_bandera_contraria() {
	//
	// ...
	//
}

bool Equipo::jugador_rodeado(int nro_jugador) {
    vector<coordenadas> vecinos;
    coordenadas pos_actual = this->posiciones[nro_jugador];
    for (int i=-1; i<2; i++){
        for (int j=-1; j<2; j++){
            if (i==0 && j==0) continue;
            if (this->belcebu->es_posicion_valida(make_pair(pos_actual.first+i, pos_actual.second+j))){
                vecinos.push_back(make_pair(pos_actual.first+i, pos_actual.second+j));
            }
        }
    }
    int l;
    for (auto &v: vecinos){
        if (!(this->belcebu->es_color_libre(this->belcebu->en_posicion(v)))){
            l++;
        }
    }
    return l == vecinos.size();
}

int Equipo::jugador_minima_distancia() {
    int min_dist = numeric_limits<int>::max();
    int nro_jug = 0;
    for (int i = 0; i < this->cant_jugadores; i++) {
        coordenadas p = this->posiciones[i];
        int dist = this->belcebu->distancia(p, this->pos_bandera_contraria);
        if (dist < min_dist) {
            min_dist = dist;
            nro_jug = i;
        }
    }
    return nro_jug;
}
int Equipo::jugador_maxima_distancia() {
    int max_dist = -1;
    int nro_jug = 0;
    for (int i = 0; i < this->cant_jugadores; i++) {
        coordenadas p = this->posiciones[i];
        int dist = this->belcebu->distancia(p, this->pos_bandera_contraria);
        if (max_dist < dist) {
            max_dist = dist;
            nro_jug = i;
        }
    }
    return nro_jug;
}