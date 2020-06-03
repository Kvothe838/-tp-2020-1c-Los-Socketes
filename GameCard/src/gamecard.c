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
	administrarNewPokemon("Prueba", 0, 0, 1); //una posicion que existe
	administrarNewPokemon("Prueba", 0, 1, 1);
	administrarNewPokemon("Prueba", 0, 2, 1);
	administrarNewPokemon("Prueba", 0, 3, 1);
	administrarNewPokemon("Prueba", 0, 4, 1);
	administrarNewPokemon("Prueba", 0, 5, 1);
	administrarNewPokemon("Pikachu", 0, 0, 1);



/*
	FILE* fp = fopen("Blocks/1.bin", "wb");

	int p[] = {3,4,5};
	int pFrom1[3];

	for(int i = 0; i < 3; i ++)
		fwrite(&p, sizeof(int), 3, fp);
	fclose(fp);

	FILE* fp = fopen("Blocks/1.bin", "rb");

	for(int i = 0; i < 3; i ++){
		fread(&pFrom1, sizeof(int), 3, fp);
		printf("P de archivo: %d, %d y %d", pFrom1[0], pFrom1[1], pFrom1[2]);
	}

	void * datos;
	FILE* fp = fopen("Blocks/1.bin", "rb");
	fread(datos, sizeof(int), 9, fp);
	int primerDato;
	memcpy(&primerDato, datos, sizeof(int));
	printf("Primer dato: %d", primerDato);
*/






	//int socket = crear_conexion_cliente(ip, puerto);

	//////////////////////////////////////////////////
	//mandarSuscripcion(socket, 3, NEW, GET, CAUGHT);


	//iniciar_servidor(ip, puerto);

	//terminar_programa(logger, config);

	return EXIT_SUCCESS;
}


