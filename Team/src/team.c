#include <shared/utils.h>
#include "teamInit.h"


int main(void) {
	Config* configTeam = malloc(sizeof(Config));
	cargarConfig(configTeam);

	Team team = inicializarTeam(configTeam->posiciones,configTeam->pertenecientes,configTeam->objetivos);

	free(configTeam);
	liberarMemoria(team);
	printf(" \n programa finalizado");
	return 0;
}















