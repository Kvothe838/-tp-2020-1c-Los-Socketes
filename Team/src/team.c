#include <shared/utils.h>
#include "teamInit.h"
#include "planificacion.h"
#include<pthread.h>
#include <string.h>

int main(void) {
	Config* configTeam = malloc(sizeof(Config));
	cargarConfig(configTeam);
	Team team = inicializarTeam(configTeam->posiciones,configTeam->pertenecientes,configTeam->objetivos);

	/* dentro de inicializarTeam se deben crear los hilos para cada entrenador, de tal forma que
	 * cuando se le asignen sus datos, se cree el hilo y quede en "pausa" (por ahora se me ocurrio el
	 * uso de semaforos con wait y signal bloqueantes: gabo),
	 */

	planificacion_fifo(team);
	free(configTeam);
	liberarMemoria(team);
	printf(" \n programa finalizado");
	return 0;
}















