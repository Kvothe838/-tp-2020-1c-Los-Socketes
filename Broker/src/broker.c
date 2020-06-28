#include <shared/utils.h>
#include "brokerColas.h"
#include "conexionBroker.h"
#include "cache/dynamicCache.h"

int main(void) {
	t_config* config;
	pthread_t threadIniciarServidor, threadEnviarMensajesSuscriptores;
	IniciarServidorArgs argumentos;

	logger = iniciar_logger("loggerBroker.log", "Broker");
	config = leer_config("configBroker.config", logger);

	argumentos.ip = config_get_string_value(config, "IP_BROKER");
	argumentos.puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "Escuchando en ip %s y puerto %s...", argumentos.ip, argumentos.puerto);

	inicializarDataBasica(config, logger);
	crearDiccionario();

	pthread_create(&threadIniciarServidor, NULL,(void*)iniciarServidor, (void*)&argumentos);
	pthread_create(&threadEnviarMensajesSuscriptores, NULL,(void*)enviarMensajesSuscriptores, NULL);
	pthread_join(threadIniciarServidor, NULL);
	pthread_join(threadEnviarMensajesSuscriptores, NULL);

	terminar_programa(logger, config);

	return 0;
}




