#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	char* ip, *puerto;
	t_log* logger;
	//t_config* config;

	logger = iniciar_logger("loggerGameCard.log", "Broker");
	//config = leer_config("configGameCard.config", logger);

	ip = "127.0.0.1";
	puerto = "6009";

	log_info(logger, "IP %s y PUERTO %s", ip, puerto);

	int socket = crear_conexion_cliente(ip, puerto);

	int* numero;
	*numero = 78;

	int resultado = enviar_mensaje(numero, sizeof(int), SUSCRIBER, socket);

	//iniciar_servidor(ip, puerto);

	//terminar_programa(logger, config);

	return EXIT_SUCCESS;
}

