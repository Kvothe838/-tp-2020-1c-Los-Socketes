#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include "conexionBinario/conexionBinario.h"
#include "ManejoDeBloques/manejoDeArchivos.h"
#include "conexionGameCard.h"
#include <stdlib.h>
#include "conexionGamecard.h";

int main(void) {
	char *ip, *puerto;
	t_log* logger = iniciar_logger("gamecard.log", "GAMECARD");
	t_config* config;

	inicializarData(logger);

	iniciar_servidor("127.0.0.1", "5001");

	/*
	inicializarData(logger);
	administrarNewPokemon("Prueba", 0, 0, 1); //12
	administrarNewPokemon("Prueba", 0, 1, 1); //24
	administrarNewPokemon("Prueba", 0, 2, 1); //36
	administrarNewPokemon("Prueba", 0, 3, 1); //48
	administrarNewPokemon("Prueba", 0, 4, 1); //60
	administrarNewPokemon("Prueba", 0, 5, 1); //72
	administrarNewPokemon("Pikachu", 0, 0, 1); //12
	

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

	//Para pruebas de suscripción con Broker. Comentar si no se usa pero no borrar plis.

	/*
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
	*/

	terminar_programa(logger, config);

	return EXIT_SUCCESS;
}





