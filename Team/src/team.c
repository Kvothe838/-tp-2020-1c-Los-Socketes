#include "team.h"

int main(void) {
	t_log* logger;

	logger = iniciar_logger("Team.log", "Team");
	t_config* config;
	config = leer_config("configTeam.config", logger);
	Config* configTeam = malloc(sizeof(Config));
	cargarConfig(configTeam, config);

	log_info(logger, "IP %s y PUERTO %s", configTeam->ip, configTeam->puerto);
	iniciar_servidor(configTeam->ip,configTeam->puerto);

/*	Team team = inicializarTeam(configTeam->posiciones,configTeam->pertenecientes,configTeam->objetivos);
	planificacion_fifo(team);
	free(configTeam);
	liberarMemoria(team);
*/


	terminar_programa(logger, config);
	printf(" \n programa finalizado");
	return 0;
}















