// proyecto realizado por Alexis y Gabriel

#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	char* ip, *puerto;
	t_log* logger;
	t_config* config;

	logger = iniciar_logger("GameBoy.log", "Broker");
	config = leer_config("GameBoy.config", logger);

	ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "IP %s y PUERTO %s", ip, puerto);

	//iniciar_servidor(ip, puerto);

	terminar_programa(logger, config);

	return EXIT_SUCCESS;
}
