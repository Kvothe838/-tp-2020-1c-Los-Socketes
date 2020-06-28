#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include "conexionBinario/conexionBinario.h"
//#include "ManejoDeBloques/manejoDeArchivos.h"
#include <stdlib.h>
#include "conexionGameCard.h"
#include "ManejoDeBloques/manejoDeArchivos.h"

int main(void) {
	t_log* logger = iniciar_logger("gamecard.log", "GAMECARD");
	t_config* config = leer_config("GameCard.config", logger);
	char *ip, *puerto;

	inicializarData(logger);

	//iniciar_servidor("127.0.0.1", "5001");
	
	/*inicializarData(logger);

	for (int i = 1; i < 5; ++i)
		administrarNewPokemon("TesteoLoco", i*100000000, i*100000000, 3);
	administrarNewPokemon("TesteoLoco", 1, 1, 1);

	administrarCatchPokemon("TesteoLoco", 400000000, 400000000);

	for (int i = 1; i < 100; ++i)
		administrarNewPokemon("Pikachu", i, i, 3);

	LocalizedPokemon * datosRecibidos = administrarGetPokemon("Pikachu");

	printf("%s\n", datosRecibidos->nombre);
	printf("%d\n", datosRecibidos->cantidadDePosiciones);

	uint32_t offset = 0;
	uint32_t *data;
	uint32_t ciclos = datosRecibidos->cantidadDePosiciones;
	while(ciclos != 0){
		ciclos--;

		memcpy(&data, (datosRecibidos->data + offset), sizeof(uint32_t));
		printf("X:%d - ", (int)data);
		offset += sizeof(uint32_t);
		memcpy(&data, (datosRecibidos->data + offset), sizeof(uint32_t));
		printf("Y:%d - ", (int)data);
		offset += sizeof(uint32_t);
		memcpy(&data, (datosRecibidos->data + offset), sizeof(uint32_t));
		printf("Cantidad:%d\n", (int)data);
		offset += sizeof(uint32_t);
	}

	free(datosRecibidos->data);
	free(datosRecibidos);*/

	//Para pruebas de suscripción con Broker. Comentar si no se usa pero no borrar plis.


	ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");
	int socketCliente = crear_conexion_cliente(ip, puerto);
	mandarSuscripcion(socketCliente, GAMECARD, 3, NEW, GET, CAUGHT);
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
			recv(socketCliente, &mensaje->tamanioContenido, sizeof(int), 0);
			recv(socketCliente, &mensaje->contenido, mensaje->tamanioContenido, 0);

			log_info(logger, "Nuevo mensaje recibido con ID %d de cola %s", mensaje->ID, tipoColaToString(mensaje->cola));

			stream = malloc(sizeof(long));
			memcpy(stream, &(mensaje->ID), sizeof(long));
			respuesta = armarPaqueteYSerializar(ACK, GAMECARD, sizeof(long), stream, &bytes);

			send(socketCliente, &respuesta, bytes, 0);

			log_info(logger, "ACK enviado para mensaje con ID %d", mensaje->ID);
		}
	}

	//liberarVariablesGlobales();

	log_destroy(logger);
	//terminar_programa(logger, config);

	return EXIT_SUCCESS;
}









