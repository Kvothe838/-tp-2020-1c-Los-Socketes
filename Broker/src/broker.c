#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	char* ip, puerto;
	t_log* logger;
	t_config* config;

	logger = iniciar_logger("loggerBroker.log", "Broker");
	config = leer_config("loggerConfig.config", logger);

	puerto = config_get_string_value(config, "PUERTO_BROKER");
	ip = config_get_string_value(config, "IP_BROKER");

	/*iniciar_servidor(ip, puerto); Esto no funca, rompe porque dice que no reconoce
	las funciones que llaman a thread. Debería reconocerlo porque la biblioteca está
	declarada en structs.h y esa está importada en todos.*/

	terminar_programa(logger, config);

	return EXIT_SUCCESS;
}
