#include <shared/utils.h>
#include "brokerColas.h"
#include "conexionBroker.h"
#include "cache/dynamicCache.h"

void imprimirTabla(t_dynamic_table_entry tabla[], t_log* logger){
	log_info(logger, "MOSTRANDO ELEMENTOS");
	for(int i = 0; i < tableSize; i++){
		if(!tabla[i].isEmpty){
			log_info(logger, "Elemento ID %d, posicion %d,  espacio %d, fecha %s",
					tabla[i].id, tabla[i].position, tabla[i].size, tabla[i].dateLastUse);
		}
	}
}

int main(void) {
	char* ip, *puerto;
	t_config* config;

	logger = iniciar_logger("loggerBroker.log", "Broker");
	config = leer_config("configBroker.config", logger);

	ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "IP %s y PUERTO %s", ip, puerto);

	initializeDataBasic(config);
	inicializarCache();
	crearDiccionario();
	iniciar_servidor(ip, puerto);
	terminar_programa(logger, config);

	return 0;
}




