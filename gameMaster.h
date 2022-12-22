#ifndef GAMEMASTER_H
#define GAMEMASTER_H

#include <tuple>
#include <cstdio>
#include <vector>
#include <mutex>
#include <semaphore.h>
#include <iostream>
#include "definiciones.h"
#include "config.h"

using namespace std;

class gameMaster {
private:
    // Atributos Privados
    int nro_ronda = 0;
    int x, y, jugadores_por_equipos;
    vector<vector<color>> tablero;
    vector<coordenadas> pos_jugadores_azules, pos_jugadores_rojos;
    color turno;
    estrategia strat;
	
    //
    //...
    //

    // Métodos privados
    color obtener_coordenadas(coordenadas coord);
    void mover_jugador_tablero(coordenadas pos_anterior, coordenadas pos_nueva, color colorEquipo);
    //
    //...
    //
 
public:
    // Atributos públicos
    gameMaster(Config config);
    void termino_ronda(color equipo); // Marca que un jugador terminó la ronda
    int mover_jugador(direccion dir, int nro_jugador);
    bool se_puede_mover(coordenadas pos_anterior, direccion dir);
    color ganador = INDEFINIDO;
    sem_t barrier;
    coordenadas pos_bandera_roja, pos_bandera_azul; //HACER PRIVDOS DE VUELTA Y ACCESIBLES POR FUNCION
    //
    //...
    //

    // Métodos públicos
    bool termino_juego();
	int getTamx();
	int getTamy();
    static int distancia(coordenadas pair1, coordenadas pair2);
    sem_t turno_rojo, turno_azul; // FIXME: Al principio necesito entrar como azul, luego puedo hacerlo por el método termino_ronda....
    color en_posicion(coordenadas coord);
    bool es_posicion_valida(coordenadas pos);
    bool es_color_libre(color color_tablero);
	coordenadas proxima_posicion(coordenadas anterior, direccion movimiento); // Calcula la proxima posición a moverse	
    void dibujame();
    void draw_board();
    color dame_equipo_actual();
    color equipo_jugando();
    coordenadas pos_bandera(color equipo);
    //
    //...
    //
};

#endif // GAMEMASTER_H