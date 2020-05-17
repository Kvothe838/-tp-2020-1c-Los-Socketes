#include <shared/utils.h>
#include "brokerColas.h"
#include "conexionBroker.h"

int main(void) {
	char* ip, *puerto;
	t_log* logger;
	t_config* config;

	logger = iniciar_logger("loggerBroker.log", "Broker");
	config = leer_config("configBroker.config", logger);

	ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger, "IP %s y PUERTO %s", ip, puerto);

	log_info(logger, "Se crea el diccionario y a continuacion muestra a los suscriptores de 'NEW'", ip, puerto);
	crearDiccionario();

	agregarSuscriptor(NEW, 3);
	agregarSuscriptor(NEW, 4);
	obtenerSuscriptoresPorCola(NEW);

	iniciar_servidor(ip, puerto);

	terminar_programa(logger, config);

	return 0;
}
