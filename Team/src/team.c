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
		log_info(logTP,"Entrenador %d realizo %d ciclos de cpu",z,getQuantum(z));
	}
}
void metricas(){
		/*
		printf("\n-------------------METRICAS-------------------------\n");
		for(int i=0;i < CANT_ENTRENADORES;i++){
			printf("\n%d -> %d",i,getQuantum(i));
		}
		*/
		log_info(logTP,"Ciclos de cpu totales: %d",sumarCiclos());
		log_info(logTP,"Ciclos de cpu de cada entrenador:");
		mostrarCiclosIndividuales();
		log_info(logTP,"Cantidad de cambios de contexto: %d",CAMBIOS_CONTEXTO);
		log_info(logTP,"Cantidad de deadlocks resueltos: %d",DEADLOCKS_RESUELTOS);
}

int main(void) {
	t_log* logger = iniciar_logger("CargarConfiguracion.log", "ConfigTeam");
	cargarConfig(logger);
	argumentos.ip = configTeam.ipTeam;
	argumentos.puerto = configTeam.puertoTeam;
	Team team = inicializarTeam(configTeam.posiciones,configTeam.pertenecientes,configTeam.objetivos);
	conexiones(team);
	planificacion();
	metricas();
	printf(" \nprograma finalizado\n");
	return 0;
}
