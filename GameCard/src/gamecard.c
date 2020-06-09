#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include "conexionBinario/conexionBinario.h"
#include "ManejoDeBloques/manejoDeArchivos.h"
#include <stdlib.h>
#include "conexionGamecard.h";



int main(void) {

	t_log* logger;
	logger = iniciar_logger("GAMECARD.log", "GAMECARD");
	inicializarData(logger);

	iniciar_servidor("127.0.0.1", "5001");


	/*
	 * administrarCatchPokemon() //devuelve un boolean para usarlo en CAUGHT_POKEMON
	 * administrarGetPokemon() //devuelve un localized y lo manda al broker
	 * administrarNewPokemon() //devuelve un APPEARED_POKEMON, nombre, X e Y
	 *
	 *
	 * */

	/*
	LocalizedPokemon * datosRecibidos = administrarGetPokemon("Testeo");
	printf("%s\n", datosRecibidos->nombre);
	printf("%d\n", datosRecibidos->cantidadDePosiciones);

	uint32_t offset = 0;
	uint32_t *data;
	uint32_t ciclos = datosRecibidos->cantidadDePosiciones;
	while(ciclos != 0){
		ciclos--;

		memcpy(&data, (datosRecibidos->data + offset), sizeof(uint32_t));
		printf("X:%d - ", data);
		offset += sizeof(uint32_t);
		memcpy(&data, (datosRecibidos->data + offset), sizeof(uint32_t));
		printf("Y:%d - ", data);
		offset += sizeof(uint32_t);
		memcpy(&data, (datosRecibidos->data + offset), sizeof(uint32_t));
		printf("Cantidad:%d\n", data);
		offset += sizeof(uint32_t);

	}*/


	/*char* ipBroker = "127.0.0.1";
	char* puertoBroker = "6009";
	int socket = crear_conexion_cliente(ipBroker, puertoBroker);*/

	//////////////////////////////////////////////////
	//mandarSuscripcion(socket, 3, NEW, GET, CAUGHT);


	//iniciar_servidor(ip, puerto);

	//terminar_programa(logger, config);

	return EXIT_SUCCESS;
}





