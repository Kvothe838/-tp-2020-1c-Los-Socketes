#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include "conexionBinario/conexionBinario.h"
#include "ManejoDeBloques/manejoDeArchivos.h"
#include <stdlib.h>
#include "conexionGameCard.h"

int main(void) {
<<<<<<< HEAD
	//char *ip, *puerto;
	t_log* logger = iniciar_logger("gamecard.log", "GAMECARD");
	//t_config* config;
=======
	char *ip, *puerto;
	t_log* logger = iniciar_logger("loggerGameCard.log", "GameCard");
	t_config* config = leer_config("configGameCard.config", logger);
>>>>>>> 64635bbe451f9b2c99c9ae0c2ae33514eea33a97

	/*inicializarData(logger);

	//administrarNewPokemon("Pikachu", 15, 10, 1);


	/*for (int i = 0; i < 10; ++i) {
		administrarNewPokemon("Pikachu", i, i, 1);
	}

	for (int i = 0; i < 10; ++i) {
		administrarNewPokemon("Pepe", i, i, 1);
	}


	administrarCatchPokemon("JOsé", 5, 5);
	administrarCatchPokemon("Pikachu", 5, 5);
	administrarCatchPokemon("Pikachu", 5, 5);
	LocalizedPokemon* poke = administrarGetPokemon("Pepe");
	printf("%d",poke->cantidadDePosiciones);
	free(poke->data);
	free(poke);*/
	/*for (int i = 0; i < 6; ++i) {
		administrarCatchPokemon("Pikachu", i, i);
	}*/
	iniciar_servidor("127.0.0.1", "5001");

<<<<<<< HEAD
		/*
=======

>>>>>>> 64635bbe451f9b2c99c9ae0c2ae33514eea33a97
	inicializarData(logger);
	administrarNewPokemon("Prueba", 0, 0, 1); //12
	administrarNewPokemon("Prueba", 0, 1, 1); //24
	administrarNewPokemon("Prueba", 0, 2, 1); //36
	administrarNewPokemon("Prueba", 0, 3, 1); //48
	administrarNewPokemon("Prueba", 0, 4, 1); //60
	administrarNewPokemon("Prueba", 0, 5, 1); //72
	administrarNewPokemon("Pikachu", 0, 0, 1); //12
	

	 * administrarCatchPokemon() //devuelve un boolean para usarlo en CAUGHT_POKEMON
	 * administrarGetPokemon() //devuelve un localized y lo manda al broker
	 * administrarNewPokemon() //devuelve un APPEARED_POKEMON, nombre, X e Y
	 *
	 *
	 *


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

	//Para pruebas de suscripción con Broker. Comentar si no se usa pero no borrar plis.


	ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");
	int socketCliente = crear_conexion_cliente(ip, puerto);
	mandarSuscripcion(socketCliente, 3, NEW, GET, CAUGHT);
	log_info(logger, "MANDADAS COLAS NEW, GET, CAUGHT");

	MensajeParaSuscriptor* mensaje = (MensajeParaSuscriptor*)malloc(sizeof(MensajeParaSuscriptor));
	OpCode codigo;
	int size;

	while(1){
		recv(socketCliente, &codigo, sizeof(OpCode), 0);
		if(codigo == NUEVO_MENSAJE_SUSCRIBER){
			void *stream, *respuesta;
			int bytes;

			recv(socketCliente, &size, sizeof(int), 0);
			recv(socketCliente, &mensaje->ID, sizeof(long), 0);
			recv(socketCliente, &mensaje->IDMensajeCorrelativo, sizeof(long), 0);
			recv(socketCliente, &mensaje->cola, sizeof(TipoCola), 0);
			recv(socketCliente, &mensaje->sizeContenido, sizeof(int), 0);
			recv(socketCliente, &mensaje->contenido, mensaje->sizeContenido, 0);

			log_info(logger, "Nuevo mensaje recibido con ID %d de cola %s", mensaje->ID, tipoColaToString(mensaje->cola));

			stream = malloc(sizeof(long));
			memcpy(stream, &(mensaje->ID), sizeof(long));
			respuesta = armarPaqueteYSerializar(ACK, sizeof(long), stream, &bytes);

			send(socketCliente, &respuesta, bytes, 0);

			log_info(logger, "ACK enviado para mensaje con ID %d", mensaje->ID);
		}
	}

	liberarVariablesGlobales();

	log_destroy(logger);
	//terminar_programa(logger, config);

	return EXIT_SUCCESS;
}






