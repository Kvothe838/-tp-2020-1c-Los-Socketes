#include "planificacion.h"

void activar_entrenador(Entrenador* persona){
	CAMBIOS_CONTEXTO += 1;
	persona->estado = EJECUTANDO;
	sem_post(&(persona->activador));
	sem_wait(&ya_termine);
	persona->estado = BLOQUEADO;
	sem_post(&siguiente);
}

void ejecucion_entrenadores(){
	while((queue_size(DISPONIBLES) > 0)||(queue_size(EJECUTADOS) > 0)||(queue_size(PREPARADOS) > 0)||(queue_size(DEADLOCKS) > 0)||(queue_size(ESPERANDO)>0)){
	//while(1){
		sem_wait(&s_ejecucion);
		activar_entrenador(queue_peek(PREPARADOS));
		sem_wait(&siguiente);
		pthread_mutex_lock(&modificar_cola_preparados);
		queue_pop(PREPARADOS);
		pthread_mutex_unlock(&modificar_cola_preparados);
		sem_post(&esperar_finalizacion);
		sem_wait(&finalizar_ejecucion);
		/*if((queue_size(ESPERANDO))==(queue_size(DEADLOCKS))==(queue_size(PREPARADOS))==(queue_size(EJECUTADOS))==(queue_size(DISPONIBLES))==0){
			printf("\nejecucion_entrenadores() FINALIZADO\n");
		}*/
	}
	printf("\nCHAU EJECUCION\n");
	pthread_exit(NULL);
}

void match_atrapar(){
	mostrarColas();
	//intercambios();
	while((queue_size(DISPONIBLES) > 0)||(queue_size(EJECUTADOS) > 0)||(queue_size(PREPARADOS) > 0)||(queue_size(DEADLOCKS) > 0)||(queue_size(ESPERANDO)>0)){
	//while(1){
		sem_wait(&hayPreparados);
		sem_post(&s_ejecucion);
		sem_wait(&esperar_finalizacion);
		//mostrarColas();
		//intercambios();
		if(queue_size(DISPONIBLES)==0 && queue_size(EJECUTADOS)>0){ // EJECUTADOS --> DISPONIBLES // TERMINAN
			while(queue_size(EJECUTADOS)>0){
				ingreso_a_colas_entrenador(queue_peek(EJECUTADOS));
				pthread_mutex_lock(&modificar_cola_ejecutados);
				queue_pop(EJECUTADOS);
				pthread_mutex_unlock(&modificar_cola_ejecutados);
			}
			//printf("\nSE ACTUALIZARON LAS LISTAS\n");
			//mostrarColas();
		}
		/*if((queue_size(ESPERANDO))==(queue_size(DEADLOCKS))==(queue_size(PREPARADOS))==(queue_size(EJECUTADOS))==(queue_size(DISPONIBLES))==0){
			printf("\nmatch_atrapar() FINALIZADO\n");
		}*/
		//mostrarColas();
		sem_post(&finalizar_ejecucion);
	}
	printf("\nCHAU MATCHEO\n");
	pthread_exit(NULL);
}
/*
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 */

void planificacion_fifo(t_log* logger,Config* configTeam){
	printf("\n---------------------------------------------------------------\n");
	
	pthread_t atrapar;
	pthread_t ejecucion;
	//pthread_t conexiones;

	log_info(logger, "IP %s y PUERTO %s", configTeam->ip, configTeam->puerto);
	//pthread_create(&conexiones,NULL,(void*)iniciar_servidor,&configTeam);
	
	pthread_create(&atrapar,NULL,(void*)match_atrapar,NULL);
	pthread_create(&ejecucion,NULL,(void*)ejecucion_entrenadores,NULL);

	pthread_join(atrapar,NULL);
	pthread_join(ejecucion,NULL);
	printf("\nSOLO FALTAN CONEXIONES\n");
	fclose(logTP);
	//pthread_detach(conexiones);
	//pthread_join(conexiones,NULL);
	printf("\nFINALIZA LA PLANIFICACION");
}
