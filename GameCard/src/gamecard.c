#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include "conexionBinario/conexionBinario.h"
#include "ManejoDeBloques/manejoDeArchivos.h"



int main(void) {

	t_log* logger;
	logger = iniciar_logger("loggerGameCard.log", "Broker");
	inicializarData(logger);




	//int socket = crear_conexion_cliente(ip, puerto);

	//////////////////////////////////////////////////
	//mandarSuscripcion(socket, 3, NEW, GET, CAUGHT);


	//iniciar_servidor(ip, puerto);

	//terminar_programa(logger, config);

	return EXIT_SUCCESS;
}


