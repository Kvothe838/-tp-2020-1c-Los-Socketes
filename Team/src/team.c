#include "team.h"

int main(void) {
	Config* configTeam = malloc(sizeof(Config));
	cargarConfig(configTeam);
	Team team = inicializarTeam(configTeam->posiciones,configTeam->pertenecientes,configTeam->objetivos);
	planificacion_fifo();
	free(configTeam);
	liberarMemoria(team);
	printf(" \nprograma finalizado");
	return 0;
}















