#include <shared/utils.h>
#include "brokerColas.h"

int main(void) {
	char* ip, *puerto;
	t_log* logger;
	t_config* config;

	logger = iniciar_logger("loggerBroker.log", "Broker");
	config = leer_config("configBroker.config", logger);

	ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger, "IP %s y PUERTO %s", ip, puerto);

	t_dictionary* diccionarioPrincipal = crearDiccionario();

	log_info(logger, "Se crea el diccionario y a continuacion muestra a los suscriptores de 'NEW'", ip, puerto);

	agregarSuscriptor(diccionarioPrincipal, "NEW", 3);
	agregarSuscriptor(diccionarioPrincipal, "NEW", 4);
	obtenerSuscriptoresPorCola(diccionarioPrincipal, "NEW");


	iniciar_servidor(ip, puerto);


	terminar_programa(logger, config);

	return 0;
}
