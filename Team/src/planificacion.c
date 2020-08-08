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
		log_info(logTP_aux,"====== sem_wait(&s_ejecucion) ======");
		sem_wait(&s_ejecucion);
		activar_entrenador(queue_peek(PREPARADOS));
		sem_wait(&siguiente);
		pthread_mutex_lock(&modificar_cola_preparados);
		queue_pop(PREPARADOS);
		pthread_mutex_unlock(&modificar_cola_preparados);
		sem_post(&esperar_finalizacion);
		sem_wait(&finalizar_ejecucion);
	}
	pthread_exit(NULL);
}

void match_atrapar(){
	while((queue_size(DISPONIBLES) > 0)||(queue_size(EJECUTADOS) > 0)||(queue_size(PREPARADOS) > 0)||(queue_size(DEADLOCKS) > 0)||(queue_size(ESPERANDO)>0)){
		log_info(logTP_aux,"====== sem_wait(&hayPreparados) ======");
		sem_wait(&hayPreparados);
		sem_post(&s_ejecucion);
		sem_wait(&esperar_finalizacion);
		if(queue_size(EJECUTADOS)>0){ // EJECUTADOS --> DISPONIBLES // TERMINAN
			while(queue_size(EJECUTADOS)>0){
				ingreso_a_colas_entrenador(queue_peek(EJECUTADOS));
				pthread_mutex_lock(&modificar_cola_ejecutados);
				queue_pop(EJECUTADOS);
				pthread_mutex_unlock(&modificar_cola_ejecutados);
			}
		}
		sem_post(&finalizar_ejecucion);
	}
	pthread_exit(NULL);
}

void planificacion(){
	pthread_t atrapar;
	pthread_t ejecucion;
	pthread_create(&atrapar,NULL,(void*)match_atrapar,NULL);
	pthread_create(&ejecucion,NULL,(void*)ejecucion_entrenadores,NULL);
	pthread_join(atrapar,NULL);
	pthread_join(ejecucion,NULL);
}
