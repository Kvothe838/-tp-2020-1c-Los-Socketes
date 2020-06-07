#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include "conexionBinario/conexionBinario.h"
#include "ManejoDeBloques/manejoDeArchivos.h"
#include<stdlib.h>



int main(void) {

	t_log* logger;
	logger = iniciar_logger("loggerGameCard.log", "GAMECARD");
	inicializarData(logger);

	for (int i = 0; i < 2; ++i) {
		administrarNewPokemon("Prueba", i, i, 1);
	}
	for (int i = 0; i < 3; ++i) {
		administrarCatchPokemon("Prueba", i, i);
	}
	administrarCatchPokemon("José", 7, 8);



	/*char* ipBroker = "127.0.0.1";
	char* puertoBroker = "6009";
	int socket = crear_conexion_cliente(ipBroker, puertoBroker);*/

	//////////////////////////////////////////////////
	//mandarSuscripcion(socket, 3, NEW, GET, CAUGHT);


	//iniciar_servidor(ip, puerto);

	//terminar_programa(logger, config);

	return EXIT_SUCCESS;
}



