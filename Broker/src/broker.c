//#include <commons/log.h>
#include <shared/utils.h>
//#include <stdio.h>
//#include <stdlib.h>

int main(void) {
	char* ip, *puerto;
	t_log* logger;
	t_config* config;

	logger = iniciar_logger("loggerBroker.log", "Broker");
	config = leer_config("configBroker.config", logger);

	ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");

	iniciar_servidor(ip, puerto);

	log_info(logger, "IP %s y PUERTO %s", ip, puerto);

	terminar_programa(logger, config);

	return EXIT_SUCCESS;
}
