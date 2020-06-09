#include <shared/utils.h>
#include "brokerColas.h"
#include "conexionBroker.h"
#include "cache/dynamicCache.h"

int main(void) {
	char* ip, *puerto;
	t_config* config;

	logger = iniciar_logger("loggerBroker.log", "Broker");
	config = leer_config("configBroker.config", logger);

	ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "IP %s y PUERTO %s", ip, puerto);

	inicializarDataBasica(config, logger);
	crearDiccionario();
	iniciarServidor(ip, puerto);
	terminar_programa(logger, config);

	return 0;
}




