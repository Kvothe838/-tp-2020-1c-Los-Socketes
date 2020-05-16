#include <shared/utils.h>
#include "teamInit.h"


int main(void) {
	t_log* logger;
	t_config* config;
	logger = iniciar_logger("Team.log", "Team");
	config = leer_config("configTeam.config", logger);
	char** posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES"); // lista de strings, ultimo elemento nulo
	char** pertenecientes = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");


	Team team = inicializarTeam(posiciones,pertenecientes,objetivos);

	printf("\n entrenador inicial id %d", team[2]->posicion[0]);


	liberarMemoria(team);
	printf(" \n programa finalizado");
	return 0;
}















