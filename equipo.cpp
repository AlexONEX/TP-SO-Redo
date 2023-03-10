#include "equipo.h"
#include <assert.h>
#include <limits>

#define endl '\n'

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
	this->vuelta_rr = true;
	assert(this->posiciones.size() == this->cant_jugadores);
	this->tiempo_busqueda = 0;
	this->jugadores_buscaron = 0;
	this->jugador_busca = -1;
	
	//this->pos_bandera_contraria = this->belcebu->pos_bandera(this->contrario);
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
	auto start = chrono::high_resolution_clock::now();
	buscar_bandera_contraria(nro_jugador);
	this->bandera_contraria_encontrada.lock();
	auto end = chrono::high_resolution_clock::now();
	this->tiempo_busqueda += chrono::duration_cast<chrono::microseconds>(end - start).count();
	this->jugadores_buscaron++;
	/*
	if(this->jugadores_buscaron == this->cant_jugadores) {
		cout << "Tiempo Busqueda: " << this->tiempo_busqueda << endl;
	}
	*/
	this->bandera_contraria_encontrada.unlock();
	while(!this->belcebu->termino_juego()) { // Chequear que no haya una race condition en gameMaster
		if(this->equipo == ROJO){
			sem_wait(&this->belcebu->turno_rojo);
		}else if(this->equipo == AZUL){
			sem_wait(&this->belcebu->turno_azul);
		}
		if(this->strat==RR){
			mt.lock();
			if(nro_jugador == 0 && this->quantum_restante==this->quantum && !this->belcebu->termino_juego()){	
				this->vuelta_rr = true;
			}
			mt.unlock();
		}
		/*
		mt.lock();
		if(this->strat==USTEDES){
			this->jugador_max_distancia = this->jugador_maxima_distancia();
		}
		mt.unlock();*/
		if(this->belcebu->termino_juego()) {
			return;
		}
		coordenadas pos_actual = this->posiciones[nro_jugador];
		direccion dir = apuntar_a(pos_actual, this->pos_bandera_contraria);
		switch(this->strat) {
			case(RR):
				while(1){
					sem_wait(&this->vec_sem[nro_jugador]);
					mt.lock();
					if(this->belcebu->termino_juego()) {
						for(int i=0; i<2*this->cant_jugadores; i++) {
							sem_post(&this->barrier);
							sem_post(&this->vec_sem[i%this->cant_jugadores]);
						}
						mt.unlock();
						return;
					}
					if(!this->vuelta_rr || this->equipo != this->belcebu->equipo_jugando()) {
						mt.unlock();
						break;
					}
					assert(this->vuelta_rr && this->equipo == this->belcebu->equipo_jugando());
					pos_actual = this->posiciones[nro_jugador];
					dir = apuntar_a(pos_actual, this->pos_bandera_contraria);					
					if(this->quantum_restante>0 && this->vuelta_rr && this->equipo == this->belcebu->equipo_jugando()) {
						this->quantum_restante--;
						if(this->belcebu->mov_habilitado(pos_actual, dir)){
							this->belcebu->mover_jugador(dir, nro_jugador);
							this->posiciones[nro_jugador] = this->belcebu->pos_jugador(this->equipo, nro_jugador);
							/**/
							if(this->belcebu->termino_juego()) {
								this->vuelta_rr = false;
								for(int i=0; i<this->cant_jugadores; i++) {
									sem_post(&this->vec_sem[i%this->cant_jugadores]);
									sem_post(&this->barrier);
								}
								this->belcebu->termino_ronda(this->equipo);
								mt.unlock();
								return;
							}
						}
						sem_post(&this->vec_sem[(nro_jugador+1)%this->cant_jugadores]);
						mt.unlock();
					}
					else if(this->quantum_restante==0){
						for(int i=0; i<this->cant_jugadores; i++) {
							sem_post(&this->vec_sem[i]);
						}
						this->quantum_restante = this->quantum;
						this->vuelta_rr = false;
						sem_post(&this->vec_sem[0]);
						mt.unlock();
						this->belcebu->termino_ronda(this->equipo);
					}
				}
				mt.lock();
				this->cant_jugadores_ya_jugaron++;
				if(this->cant_jugadores_ya_jugaron == this->cant_jugadores){
					this->cant_jugadores_ya_jugaron = 0;
					this->vuelta_rr = false;
					for(int i=0; i<this->cant_jugadores; i++){
						sem_post(&this->barrier);
					}
				}
				mt.unlock();
				assert(this->cant_jugadores_ya_jugaron <= this->cant_jugadores && this->quantum_restante == this->quantum && !this->vuelta_rr);
				sem_post(&this->belcebu->barrier);
				sem_wait(&this->barrier);

				break;

			case(SECUENCIAL):
				mt.lock();
				if(this->belcebu->mov_habilitado(pos_actual, dir) && this->belcebu->termino_juego() == false) {
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
				assert(this->posiciones[nro_jugador] == this->belcebu->pos_jugador(this->equipo, nro_jugador));				
				mt.unlock();
				sem_wait(&this->barrier);
				sem_post(&this->belcebu->barrier);
				break;

			case(SHORTEST):
				//
				// ...
				//
				mt.lock();
				if(nro_jugador==this->jugador_min_distancia && this->belcebu->mov_habilitado(pos_actual, dir) && this->equipo==this->belcebu->equipo_jugando()) {
					this->belcebu->mover_jugador(dir, nro_jugador);
					this->posiciones[nro_jugador] = this->belcebu->proxima_posicion(pos_actual, dir);
					assert(this->posiciones[nro_jugador] == this->belcebu->pos_jugador(this->equipo, nro_jugador));
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
				if(nro_jugador==this->jugador_max_distancia && this->equipo==this->belcebu->equipo_jugando()) {
					pos_actual = this->belcebu->pos_jugador(this->equipo, nro_jugador);
					dir = this->apuntar_a(pos_actual, this->pos_bandera_contraria);
					int i=0;
					while(this->belcebu->mov_habilitado(pos_actual, dir) && i<this->quantum){
						this->belcebu->mover_jugador(dir, nro_jugador);
						this->posiciones[nro_jugador] = this->belcebu->pos_jugador(this->equipo, nro_jugador);
						pos_actual = this->posiciones[nro_jugador];
						dir = this->apuntar_a(pos_actual, this->pos_bandera_contraria);
						i++;
					}
				}
				this->cant_jugadores_ya_jugaron++;
				if(this->cant_jugadores_ya_jugaron == this->cant_jugadores || this->belcebu->termino_juego()) {
					this->cant_jugadores_ya_jugaron = 0;
					for(int i=0; i<this->cant_jugadores; i++) {
						sem_post(&this->barrier);
					}
					this->jugador_max_distancia = this->jugador_maxima_distancia();
					this->belcebu->termino_ronda(this->equipo);
				}
				mt.unlock();
				sem_wait(&this->barrier);
				sem_post(&this->belcebu->barrier);
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
	else if(this->strat==SHORTEST ){
		this->jugador_min_distancia = this->jugador_minima_distancia();
	}
	else if(this->strat==USTEDES){
		this->jugador_max_distancia = this->jugador_maxima_distancia();
	}
	for(int i=0; i < cant_jugadores; i++) {
		jugadores.emplace_back(thread(&Equipo::jugador, this, i)); 
	}
}

void Equipo::terminar() {
	for(auto &t:jugadores){
		t.join();
	}	
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
    int nro_jug = -1;
    for (int i = 0; i < this->cant_jugadores; i++) {
        coordenadas p = this->posiciones[i];
        int dist = this->belcebu->distancia(p, this->pos_bandera_contraria);
        if (max_dist < dist) {
            max_dist = dist;
            nro_jug = i;
        }
    }
	assert(nro_jug != -1);
    return nro_jug;
}

void Equipo::buscar_bandera_contraria(int nro_jugador) {
	int tam_x = belcebu->getTamx(); 
	int tam_y = belcebu->getTamy();
	int recorrido_de_casilleros = (tam_x * tam_y) / cant_jugadores;
	int start = nro_jugador * recorrido_de_casilleros;
	int end = (nro_jugador + 1) * recorrido_de_casilleros; 
	
	for(int i = start; i < end; i++){
		if(equipo == ROJO){
			if (this->belcebu->en_posicion(make_pair(i/tam_y, i%tam_y)) == BANDERA_AZUL){
				this->pos_bandera_contraria = make_pair(i/tam_y, i%tam_y);
				assert(this->belcebu->pos_bandera(this->contrario) == this->pos_bandera_contraria);
				bandera_contraria_encontrada.unlock();
				return;
			}
		} else {
			if (this->belcebu->en_posicion(make_pair(i/tam_y, i%tam_y)) == BANDERA_ROJA){
				this->pos_bandera_contraria = make_pair(i/tam_y, i%tam_y);
				assert(this->belcebu->pos_bandera(this->contrario) == this->pos_bandera_contraria);
				bandera_contraria_encontrada.unlock();
				return; 
			}
		}
	}

	// Division no entera, no se encontro el la bandera contraria
	// y ultimo jugador recorre mas casilleros. 
	if (nro_jugador == cant_jugadores -1 && pos_bandera_contraria == make_pair(-1,-1)){
		for(int i = end; i < tam_x * tam_y; i++){
			if(equipo == ROJO){
				if (this->belcebu->en_posicion(make_pair(i/tam_y, i%tam_y)) == BANDERA_AZUL){
					pos_bandera_contraria = make_pair(i/tam_y, i%tam_y);
					bandera_contraria_encontrada.unlock();
					return;
				}
			} else {
				if (this->belcebu->en_posicion(make_pair(i/tam_y, i%tam_y)) == BANDERA_ROJA){
					pos_bandera_contraria = make_pair(i/tam_y, i%tam_y);
					bandera_contraria_encontrada.unlock();
					return; 
				}
			}
		}	
	}
}

void Equipo::buscar_bandera_contraria_secuencial(){
	auto start = std::chrono::high_resolution_clock::now();
	int tam_x = belcebu->getTamx();
	int tam_y = belcebu->getTamy();
	for(int i = 0; i < tam_x; i++){
		for(int j = 0; j < tam_y; j++){
			if(equipo == ROJO){
				if (this->belcebu->en_posicion(make_pair(i, j)) == BANDERA_AZUL){
					this->pos_bandera_contraria = make_pair(i, j);
					auto end = std::chrono::high_resolution_clock::now();
					/*
					cout << "Tiempo Secuencial " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
					<< " " << this->equipo  << endl;
					*/
					return;
				}
			} else {
				if (this->belcebu->en_posicion(make_pair(i, j)) == BANDERA_ROJA){
					this->pos_bandera_contraria = make_pair(i, j);
					auto end = std::chrono::high_resolution_clock::now();
					/*
					cout << "Tiempo Secuencial " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
					<< " " << this->equipo  << endl;
					*/
					return;
				}
			}
		}
	}
}