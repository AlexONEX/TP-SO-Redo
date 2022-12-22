#ifndef CONFIG_H
#define CONFIG_H

#include "definiciones.h"
#include <vector>
#include <iostream>
using namespace std;
class Config{
    public:
        Config();
        vector<coordenadas> pos_rojo, pos_azul;
        coordenadas bandera_roja, bandera_azul;
        int cantidad_jugadores;
        int x,y;

    private:
        
};
#endif // CONFIG_H
