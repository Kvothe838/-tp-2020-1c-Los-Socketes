#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include "conexionBinario/conexionBinario.h"
#include "ManejoDeBloques/manejoDeArchivos.h"
#include "conexionGameCard.h"

int main(void) {
	char* ip, *puerto;
	t_log* logger;
	t_config* config;

	logger = iniciar_logger("loggerGameCard.log", "GameCard");
	config = leer_config("configGameCard.config", logger);
	ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");

	/*
	inicializarData(logger);
	administrarNewPokemon("Prueba", 0, 0, 1); //12
	administrarNewPokemon("Prueba", 0, 1, 1); //24
	administrarNewPokemon("Prueba", 0, 2, 1); //36
	administrarNewPokemon("Prueba", 0, 3, 1); //48
	administrarNewPokemon("Prueba", 0, 4, 1); //60
	administrarNewPokemon("Prueba", 0, 5, 1); //72
	administrarNewPokemon("Pikachu", 0, 0, 1); //12
	*/



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

	//Para pruebas de suscripción con Broker. Comentar si no se usa pero no borrar plis.


	log_info(logger, "SOCKET GAMECARD: %d", socket);
	int socket = crear_conexion_cliente(ip, puerto);
	mandarSuscripcion(socket, 3, NEW, GET, CAUGHT);
	log_info(logger, "MANDADAS COLAS NEW, GET, CAUGHT");


	MensajeParaSuscriptor* mensaje = (MensajeParaSuscriptor*)malloc(sizeof(MensajeParaSuscriptor));
	OpCode codigo;
	int size;

	while(1){
		recv(socket, &codigo, sizeof(OpCode), 0);
		if(codigo == NUEVO_MENSAJE_SUSCRIBER){
			void *stream, *respuesta;
			int bytes;

			recv(socket, &size, sizeof(int), 0);
			recv(socket, &mensaje->IDMensaje, sizeof(long), 0);
			recv(socket, &mensaje->IDMensajeCorrelativo, sizeof(long), 0);
			recv(socket, &mensaje->cola, sizeof(TipoCola), 0);
			recv(socket, &mensaje->sizeContenido, sizeof(int), 0);
			recv(socket, &mensaje->contenido, mensaje->sizeContenido, 0);

			stream = malloc(sizeof(long));
			memcpy(stream, &(mensaje->IDMensajeCorrelativo), sizeof(long));
			respuesta = armarPaqueteYSerializar(ACK, sizeof(long), stream, &bytes);

			send(socket, &respuesta, bytes, 0);
		}
	}

	terminar_programa(logger, config);

	return EXIT_SUCCESS;
}


