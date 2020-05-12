// proyecto realizado por Alexis y Gabriel

//#include <commons/log.h>
#include <shared/utils.h>
//#include <stdio.h>
//#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc != 4) {
	 fprintf(stderr,"Formato de ejecución: ./gameboy [PROCESO] [TIPO_MENSAJE] [ARGUMENTOS]*\n");
	 exit(1);
	 }

	//CREO LAS VARIABLES PARA LAS 3 CONEXIONES: BROKER, TEAM Y GAMECARD
	//int conexion_broker, conexion_team, conexion_gamecard;
	char *ip_broker, *ip_team, *ip_gamecard;
	char *puerto_broker, *puerto_team, *puerto_gamecard;

	t_log* logger;
	t_config* config;


	logger = iniciar_logger("GameBoy.log", argv[1]);
	config = leer_config("GameBoy.config", logger);

	ip_broker = config_get_string_value(config, "IP_BROKER");
	puerto_broker = config_get_string_value(config, "PUERTO_BROKER");

	ip_team = config_get_string_value(config, "IP_TEAM");
	puerto_team = config_get_string_value(config, "PUERTO_TEAM");

	ip_gamecard = config_get_string_value(config, "IP_GAMECARD");
	puerto_gamecard = config_get_string_value(config, "PUERTO_GAMECARD");


	log_info(logger, "IP_BROKER %s y PUERTO_BROKER %s", ip_broker, puerto_broker);

	log_info(logger, "IP_TEAM %s y PUERTO_TEAM %s", ip_team, puerto_team);

	log_info(logger, "IP_GAMECARD %s y PUERTO_GAMECARD %s", ip_gamecard, puerto_gamecard);


	//iniciar_servidor(ip, puerto);

	terminar_programa(logger, config);

	return EXIT_SUCCESS;
}
