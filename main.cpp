#include "gameMaster.h"
#include "equipo.h"
#include "definiciones.h"
#include "config.h"
#include <assert.h>
using namespace std;

estrategia strat = USTEDES;
int main(int argc, char *argv[]){
    //receive strat trough stdin
    strat = (estrategia) atoi(argv[1]);
    Config config = *(new Config());
    gameMaster belcebu = gameMaster(config);

    int quantum = rand() % 9*config.cantidad_jugadores + 1; 
    //quantum<config.cantidad_jugadores ? quantum = config.cantidad_jugadores : quantum = quantum;
	//int quantum = config.cantidad_jugadores/2;
    //int quantum = 1;
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
