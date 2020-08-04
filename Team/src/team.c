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

int main(void) {
	Config* configTeam = malloc(sizeof(Config));
	t_log* logger = iniciar_logger("PRUEBA.log", "TEAM");
	//pthread_t threadIniciarServidor;
	IniciarServidorArgs argumentos;

	logger = iniciar_logger("Team.log", "Team");
	cargarConfig(configTeam, logger);

	argumentos.ip = configTeam->ipTeam;
	argumentos.puerto = configTeam->puertoTeam;

	ipBroker = configTeam->ip;
	puertoBroker = configTeam->puerto;
	Team team = inicializarTeam(configTeam->posiciones,configTeam->pertenecientes,configTeam->objetivos);
	//log_info(logger,"VOY A INGRESAR A PLANIFICAR");
	pthread_t planificacion;
	pthread_create(&planificacion, NULL,(void*)planificacion_fifo,NULL);
	pthread_join(planificacion,NULL);

	//fclose(logTP);
	//log_info(logger,"VOY A INGRESAR A CONEXIONES");
	conexiones(configTeam,logger, team, argumentos);
	//log_info(logger,"YA INICIALICE A TEAM");
	//pthread_create(&threadIniciarServidor, NULL,(void*)iniciarServidorTeam, (void*)&argumentos);

	//Team team = inicializarTeam(configTeam->posiciones,configTeam->pertenecientes,configTeam->objetivos);


	//pthread_join(threadIniciarServidor, NULL);

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




