#include "gameMaster.h"
#include "equipo.h"
#include "definiciones.h"
#include "config.h"
#include <assert.h>
using namespace std;

const estrategia strat = RR;
const int quantum = 4;

int main(){
    Config config = *(new Config());
    gameMaster belcebu = gameMaster(config);

	// Creo equipos (lanza procesos)
    strat == USTEDES ? assert(config.cantidad_jugadores>1): assert(config.cantidad_jugadores>0);
	Equipo rojo(&belcebu, ROJO, strat, config.cantidad_jugadores, quantum, config.pos_rojo);
	Equipo azul(&belcebu, AZUL, strat, config.cantidad_jugadores, quantum, config.pos_azul);
    
    rojo.comenzar();
    azul.comenzar();
	rojo.terminar();
	azul.terminar();	
    //belcebu.play();

    belcebu.ganador != EMPATE ? cout << "Bandera capturada por el equipo "<< belcebu.ganador << ". Felicidades!" << endl : cout << "Empate!" << endl;

}
