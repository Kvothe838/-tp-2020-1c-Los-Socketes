#include "../shared/utils.h"

t_log* iniciar_logger(char* nombreLogger, char* nombrePrograma)
{
	t_log* logger;

	if((logger = log_create(nombreLogger, nombrePrograma, true, LOG_LEVEL_INFO)) == NULL){
		printf("Error al crear el logger.");
		exit(-1);
	}

	return logger;
}

t_log* iniciarLoggerSinConsola(char* nombreLogger, char* nombrePrograma)
{
	t_log* logger;

	if((logger = log_create(nombreLogger, nombrePrograma, false, LOG_LEVEL_INFO)) == NULL){
		printf("Error al crear el logger.");
		exit(-1);
	}

	return logger;
}

t_config* leer_config(char* nombreConfig, t_log* logger)
{
	t_config* config;

	if((config = config_create(nombreConfig)) == NULL){
		log_info(logger, "Error leyendo configuración: '%s'", nombreConfig);
		exit(-1);
	}

	return config;
}

void terminar_programa(t_log* logger, t_config* config)
{
	if(logger != NULL){
		log_destroy(logger);
	}

	if(config != NULL){
		config_destroy(config);
	}
}
