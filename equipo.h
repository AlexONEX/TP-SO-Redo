#ifndef EQUIPO_H
#define EQUIPO_H

#include <semaphore.h>
#include <chrono>
#include <vector>
#include <thread>
#include <atomic>
#include "definiciones.h"
#include "gameMaster.h"
#include <unistd.h>
using namespace std;

class Equipo {
	private:

		// Atributos Privados 
		gameMaster *belcebu; 
		color contrario, equipo, bandera_contraria;
		estrategia strat;
		int cant_jugadores, quantum, quantum_restante;
		vector<thread> jugadores;
		int cant_jugadores_ya_jugaron;
		vector<coordenadas> posiciones;
		coordenadas pos_bandera_contraria;
		//
		// ...
		//

		// Métodos privados 
		direccion apuntar_a(coordenadas pos2, coordenadas pos1);
		void jugador(int nro_jugador);
		void buscar_bandera_contraria(int nro_jugador);
		//
		// ...
		//
	public:
		Equipo(gameMaster *belcebu, color equipo, 
				estrategia strat, int cant_jugadores, int quantum, vector<coordenadas> posiciones);
		void comenzar();
		void terminar();
		int jugador_minima_distancia();
		int jugador_maxima_distancia();	
		bool jugador_rodeado(int nro_jugador);

		// crear jugadores
		int jugador_min_distancia;
		int jugador_max_distancia;
		bool vuelta_rr;
		vector<sem_t> vec_sem;
		sem_t barrier;
		sem_t barrier_ustedes;
		mutex bandera_contraria_encontrada;
};
#endif // EQUIPO_H