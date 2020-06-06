#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include "conexionGameCard.h"

int main(void) {
	char* ip, *puerto;
	t_log* logger;
	t_config* config;

	logger = iniciar_logger("loggerGameCard.log", "Broker");
	config = leer_config("configGameCard.config", logger);

	ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "IP %s y PUERTO %s", ip, puerto);

	int socket = crear_conexion_cliente(ip, puerto);

	log_info(logger, "SOCKET GAMECARD: %d", socket);

	mandarSuscripcion(socket, 3, NEW, GET, CAUGHT);
	log_info(logger, "MANDADAS COLAS NEW, GET, CAUGHT");

	MensajeParaSuscriptor* mensaje = (MensajeParaSuscriptor*)malloc(sizeof(MensajeParaSuscriptor));

	while(1){
		recv(socket, &mensaje->IDMensaje, sizeof(long), 0);
		log_info(logger, "RECIBÍ MENSAJE CON ID %d", mensaje->IDMensaje);
		recv(socket, &mensaje->IDMensajeCorrelativo, sizeof(long), 0);
		log_info(logger, "RECIBÍ MENSAJE CON ID CORRELATIVO %d", mensaje->IDMensajeCorrelativo);
		recv(socket, &mensaje->cola, sizeof(TipoCola), 0);
		log_info(logger, "RECIBÍ MENSAJE DE COLA %s", tipoColaToString(mensaje->cola));
		recv(socket, &mensaje->sizeContenido, sizeof(int), 0);
		log_info(logger, "EL SIZE ES DE %d", mensaje->sizeContenido);
		recv(socket, &mensaje->contenido, mensaje->sizeContenido, 0);
		log_info(logger, "RECIBÍ CONTENIDO");

		Paquete* paquete = malloc(sizeof(Paquete));

		paquete->codigoOperacion = ACK;
		paquete->buffer = (Buffer*)malloc(sizeof(Buffer));
		paquete->buffer->size = sizeof(long);
		paquete->buffer->stream = malloc(paquete->buffer->size);
		memcpy(paquete->buffer->stream, &(mensaje->IDMensajeCorrelativo), sizeof(long));
		int bytes;
		void* respuesta = serializar_paquete(paquete, &bytes);
		log_info(logger, "Envío confirmación");
		send(socket, &respuesta, bytes, 0);
		log_info(logger, "Confirmación enviada");
	}

	//iniciar_servidor(ip, puerto);

	//terminar_programa(logger, config);

	return EXIT_SUCCESS;
}

