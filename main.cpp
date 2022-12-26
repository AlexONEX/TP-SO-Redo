#include "gameMaster.h"
#include "equipo.h"
#include "definiciones.h"
#include "config.h"
#include <assert.h>
using namespace std;
int quantum;

estrategia strat = RR;
int main(int argc, char *argv[]){
    //receive strat trough stdin
    strat = (estrategia) atoi(argv[1]);
    Config config = *(new Config());
    gameMaster belcebu = gameMaster(config);
    // Randomizamos el quantum
    srand(time(NULL));
    for(int i = 0; i < 5; i++){
        quantum += rand() % 100;
    }
    quantum = (quantum % (5*config.cantidad_jugadores));

    // Por enunciado si quantum < cantidad de jugadores, quantum = cantidad de jugadores
    if(quantum < config.cantidad_jugadores && strat == RR){
        quantum = config.cantidad_jugadores;
	}

    // Creo equipos (lanza procesos)
    strat == USTEDES ? assert(config.cantidad_jugadores>1): assert(config.cantidad_jugadores>0);
	
    Equipo rojo(&belcebu, ROJO, strat, config.cantidad_jugadores, quantum, config.pos_rojo);
	Equipo azul(&belcebu, AZUL, strat, config.cantidad_jugadores, quantum, config.pos_azul);

    rojo.buscar_bandera_contraria_secuencial();
    azul.buscar_bandera_contraria_secuencial();

    rojo.comenzar();
    azul.comenzar();
	rojo.terminar();
	azul.terminar();	
    //belcebu.play();

    belcebu.ganador != EMPATE ? cout << "Gano el equipo "<< belcebu.ganador << endl : cout << "Empate!" << endl;

}
