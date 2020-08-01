#include "team.h"

int sumarCiclos(){
	int suma=0;

	for(int i=0;i < CANT_ENTRENADORES;i++){
		suma += getQuantum(i);
	}

	return suma;
}
void mostrarCiclosIndividuales(){
	for(int z=0;z<CANT_ENTRENADORES;z++){
		fprintf(metricasTP,"Entrenador %d realizo %d ciclos de cpu\n",z,getQuantum(z));
	}
}
void metricas(){
	printf("\n-------------------METRICAS-------------------------\n");
		for(int i=0;i < CANT_ENTRENADORES;i++){
			printf("\n%d -> %d",i,getQuantum(i));
		}
		metricasTP  = fopen ("/home/utnso/Logs/metricas.txt", "w");
		fprintf(metricasTP,"Ciclos de cpu totales: %d\n",sumarCiclos());
		fprintf(metricasTP,"Ciclos de cpu de cada entrenador:\n");
		mostrarCiclosIndividuales();
		fprintf(metricasTP,"Cantidad de cambios de contexto: %d\n",CAMBIOS_CONTEXTO);
		fprintf(metricasTP,"Cantidad de deadlocks resueltos: %d\n",DEADLOCKS_RESUELTOS);
		fclose(metricasTP);
}

int main(void) {
	Config* configTeam = malloc(sizeof(Config));
	t_log* logger = iniciar_logger("PRUEBA.log", "TEAM");
	logger = iniciar_logger("Team.log", "Team");
	cargarConfig(configTeam, logger);
	Team team = inicializarTeam(configTeam->posiciones,configTeam->pertenecientes,configTeam->objetivos);
	conexiones(configTeam,logger);

	//printf("\n--------------------------------------------\n");
	//planificacion_fifo(logger,configTeam);
	//metricas();

	//free(configTeam);
	//liberarTeam(team);
	printf(" \nprograma finalizado\n");
	return 0;
}

/* COSAS PARA HACER =
1) PERMITIR QUE EL NOMBRE DEL POKEMON TENGA AL MENOS 4 LETRAS (Onixx --> Onix)
2) REALIZAR CONEXIONES CON BROKER
3) REALIZAR CONEXIONES CON GAMEBOY A PARTE DE BROKER
4) LIMPIAR MEMORY LEAKS UNA VEZ QUE TODO FUNCIONE, TAMBIEN LIMPIAR VARIABLES NO USADAS
5) CUANDO SE INGRESAN POKEMONS Y NO HAY ENTRENADORES DISPONIBLES AUN --> HACER SEMAFORO PARA QUE SE PLANIFIQUE DESPUES
*/




