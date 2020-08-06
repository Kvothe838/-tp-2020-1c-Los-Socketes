#include "planificacion.h"
t_log* logPlanificacion;
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
		log_info(logPlanificacion,"====== sem_wait(&s_ejecucion) ======");
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
	log_info(logPlanificacion,"pthread_exit(EJECUCION)");
	seguir_abierto_servidor=0;
	pthread_exit(NULL);
}

void match_atrapar(){
	//mostrarColas();
	//intercambios();
	while((queue_size(DISPONIBLES) > 0)||(queue_size(EJECUTADOS) > 0)||(queue_size(PREPARADOS) > 0)||(queue_size(DEADLOCKS) > 0)||(queue_size(ESPERANDO)>0)){
	//while(1){
		//mostrarColas();
		log_info(logPlanificacion,"====== sem_wait(&hayPreparados) ======");
		sem_wait(&hayPreparados);
		sem_post(&s_ejecucion);
		sem_wait(&esperar_finalizacion);
		//mostrarColas();
		//intercambios();
		//if(queue_size(DISPONIBLES)==0 && queue_size(EJECUTADOS)>0){ // EJECUTADOS --> DISPONIBLES // TERMINAN
		if(queue_size(EJECUTADOS)>0){ // EJECUTADOS --> DISPONIBLES // TERMINAN
			while(queue_size(EJECUTADOS)>0){
				ingreso_a_colas_entrenador(queue_peek(EJECUTADOS));
				pthread_mutex_lock(&modificar_cola_ejecutados); // SACAR
				queue_pop(EJECUTADOS);
				pthread_mutex_unlock(&modificar_cola_ejecutados); // SACAR
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
	log_info(logPlanificacion,"pthread_exit(MATCHEAR)");
	pthread_exit(NULL);
}
/*
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 */

void planificacion_fifo(){
	//printf("\n---------------------------------------------------------------\n");
	logPlanificacion = iniciar_logger("logPlanificacion.log","Planificacion");
	log_info(logPlanificacion,"INICIA LA PLANIFICACION");
	pthread_t atrapar;
	pthread_t ejecucion;
	//pthread_t conexiones;
	//log_info(logger, "IP %s y PUERTO %s", configTeam->ip, configTeam->puerto);
	//pthread_create(&conexiones,NULL,(void*)iniciar_servidor,&configTeam);
	pthread_create(&atrapar,NULL,(void*)match_atrapar,NULL);
	log_info(logPlanificacion,"ENTRO A match_atrapar");
	pthread_create(&ejecucion,NULL,(void*)ejecucion_entrenadores,NULL);
	log_info(logPlanificacion,"ENTRO A ejecucion_entrenadores");
	pthread_join(atrapar,NULL);
	pthread_join(ejecucion,NULL);


	//pthread_join(ejecucion,NULL);
	//printf("\nSOLO FALTAN CONEXIONES\n");

	//pthread_detach(conexiones);
	//pthread_join(conexiones,NULL);
}
