#include "team.h"
/*
int sumarCiclos(){
	int suma=0;

	for(int i=0;i < CANT_ENTRENADORES;i++){
		suma += getQuantum(i);
	}

	return suma;
}
void mostrarCiclosIndividuales(){
	for(int z=0;z<CANT_ENTRENADORES;z++){
		//fprintf(logTPmetricasTP,"Entrenador %d realizo %d ciclos de cpu\n",z,getQuantum(z));
	}
}
void metricas(){
	printf("\n-------------------METRICAS-------------------------\n");
		for(int i=0;i < CANT_ENTRENADORES;i++){
			printf("\n%d -> %d",i,getQuantum(i));
		}
		metricasTP  = fopen ("/home/utnso/Logs/metricas.txt", "w");
		//fprintf(logTPmetricasTP,"Ciclos de cpu totales: %d\n",sumarCiclos());
		//fprintf(logTPmetricasTP,"Ciclos de cpu de cada entrenador:\n");
		mostrarCiclosIndividuales();
		//fprintf(logTPmetricasTP,"Cantidad de cambios de contexto: %d\n",CAMBIOS_CONTEXTO);
		//fprintf(logTPmetricasTP,"Cantidad de deadlocks resueltos: %d\n",DEADLOCKS_RESUELTOS);
		fclose(metricasTP);
}
*/
int main(void) {
	t_log* logger = iniciar_logger("CargarConfiguracion.log", "ConfigTeam");
	cargarConfig(logger);
	argumentos.ip = configTeam.ipTeam;
	argumentos.puerto = configTeam.puertoTeam;
	Team team = inicializarTeam(configTeam.posiciones,configTeam.pertenecientes,configTeam.objetivos);
	conexiones(team);
	planificacion();
	printf(" \nprograma finalizado\n");
	return 0;
}