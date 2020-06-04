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

	mandarSuscripcion(socket, 3, NEW, GET, CAUGHT);
	log_info(logger, "MANDADAS COLAS NEW, GET, CAUGHT");

	TipoCola cola;
	while(1){
		recv(socket, &cola, sizeof(TipoCola), 0);
		log_info(logger, "RECIBÍ COLA %d", cola);
		int recibido = 1;
		send(socket, &recibido, sizeof(int), 0);
		log_info(logger, "Envío confirmación");
	}

	//iniciar_servidor(ip, puerto);

	//terminar_programa(logger, config);

	return EXIT_SUCCESS;
}

