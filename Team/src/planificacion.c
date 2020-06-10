#include "planificacion.h"
// HILO PARA TOMAR ENTRENADORES Y EJECUTARLOS, EXISTE SIEMPRE

void activar_entrenador(Entrenador* persona){
	persona->estado = EJECUTANDO;
	sem_post(&(persona->activador));
	sem_wait(&ya_termine);
	sem_post(&siguiente);

}

void ejecucion_entrenadores(){
	while((queue_size(DISPONIBLES) > 0)||(queue_size(PREPARADOS) > 0)){
		sem_wait(&s_ejecucion);
		activar_entrenador(queue_peek(PREPARADOS));
		sem_wait(&siguiente);
		pthread_mutex_lock(&modificar_cola_preparados);
		queue_pop(PREPARADOS);
		pthread_mutex_unlock(&modificar_cola_preparados);
		if(queue_size(PREPARADOS)>0){
			sem_post(&s_ejecucion);
		}else{
			sem_post(&rellenar_colas);
		}
	}
	pthread_exit(NULL);
}

void asignar_movimiento(Entrenador* entrenador,int mov_x,int mov_y,Pokemon* pokemon){
	entrenador->intentar_atrapar = pokemon;
	entrenador->movimiento[0]=mov_x;
	entrenador->movimiento[1]=mov_y;
	entrenador->estado = LISTO;
	pthread_mutex_lock(&modificar_cola_preparados);
	queue_push(PREPARADOS,entrenador);
	pthread_mutex_unlock(&modificar_cola_preparados);
}

int get_id(Entrenador* persona){
	return persona->idEntrenador;
}
// HILO PLANIFICAR ENTRENADORES Y POKEMONS (ATRAPAR), EXISTE MIENTRAS COLAS DISPONIBLES Y POKEMONS > 0 AMBAS
void entrenador_mas_cercano(Pokemon* pokemon){
	pthread_mutex_lock(&modificar_cola_disponibles);
	int indice_menor=0;int mov_x;int mov_y;
	float masCerca = distancia(get_posicion((list_get(DISPONIBLES->elements,0)),0),get_posicion((list_get(DISPONIBLES->elements,0)),1),pokemon->x,pokemon->y);
	mov_x = pokemon->x - get_posicion((list_get(DISPONIBLES->elements,0)),0);
	mov_y = pokemon->y - get_posicion((list_get(DISPONIBLES->elements,0)),1);
	for(int i=0;i<queue_size(DISPONIBLES);i++){
		printf("\ndistancia del entrenador %d = %.2f",get_id(list_get(DISPONIBLES->elements,i)),distancia(get_posicion((list_get(DISPONIBLES->elements,i)),0),get_posicion((list_get(DISPONIBLES->elements,i)),1),pokemon->x,pokemon->y));
		if(masCerca > distancia(get_posicion((list_get(DISPONIBLES->elements,i)),0),get_posicion((list_get(DISPONIBLES->elements,i)),1),pokemon->x,pokemon->y)){
			masCerca = distancia(get_posicion((list_get(DISPONIBLES->elements,i)),0),get_posicion((list_get(DISPONIBLES->elements,i)),1),pokemon->x,pokemon->y);
			indice_menor=i;
			mov_x = pokemon->x - get_posicion((list_get(DISPONIBLES->elements,i)),0);
			mov_y = pokemon->y - get_posicion((list_get(DISPONIBLES->elements,i)),1);
		}
	}
	printf("\n%d atrapo a %s",get_id(list_get(DISPONIBLES->elements,indice_menor)),pokemon->nombre);
	asignar_movimiento(list_get(DISPONIBLES->elements,indice_menor),mov_x,mov_y,pokemon);


	list_remove(DISPONIBLES->elements,indice_menor);
	pthread_mutex_unlock(&modificar_cola_disponibles);

	pthread_mutex_lock(&modificar_cola_pokemons);
	queue_pop(POKEMONS);
	pthread_mutex_unlock(&modificar_cola_pokemons);
}

void llegada_pokemons(){
	char pokemon[10];int x;int y;
	printf("\nDatos pokemon nuevo: ");
		scanf( "%s", pokemon );
		scanf("%d",&x);
		scanf("%d",&y);
		printf("\npokemon aparecido: %s // %d // %d",pokemon,x,y);
		while(strcmp(pokemon,"nada")!=0){
			if(loNecesito(pokemon)==1){
				agregar_pokemon_cola(pokemon,x,y);
			}else{printf("\n no necesito a este pokemon.");}

			printf("\nDatos pokemon nuevo: ");
			scanf( "%s", pokemon );
			scanf("%d",&x);
			scanf("%d",&y);
			printf("\npokemon aparecido: %s // %d // %d",pokemon,x,y);
		}
		printf("\ntam. DISPONIBLES: %d",queue_size(DISPONIBLES));
		printf("\ntam. POKEMONS: %d",queue_size(POKEMONS));
}

void match_atrapar(){
	while((queue_size(DISPONIBLES) > 0)||(queue_size(PREPARADOS) > 0)||(queue_size(POKEMONS)>0)){
		sem_wait(&rellenar_colas);
		if((queue_size(DISPONIBLES) > 0)||(queue_size(PREPARADOS) > 0)){
			if(queue_size(POKEMONS)==0){
				// este seria el equivalente a que lleguen los pokemons por mensaje de gameboy, se ejecutan de a tandas
				//(ej: se mandan 4 pokemons, se analizan esos 4 casos, luego se envian mas pokemons por mensaje)
				printf("\nVOY A DAR MAS\n");
				printf("\ntam. DISPONIBLES: %d",queue_size(DISPONIBLES));
				printf("\ntam. PREPARADOS: %d",queue_size(PREPARADOS));
				llegada_pokemons();
			}
			printf("\n---------------------voy a atrapar a %s---------------------",retornarNombrePosta(queue_peek(POKEMONS)));
			entrenador_mas_cercano(queue_peek(POKEMONS));
			printf("\ntam. DISPONIBLES: %d",queue_size(DISPONIBLES));
			printf("\ntam. POKEMONS: %d",queue_size(POKEMONS));
			printf("\ntam. PREPARADOS: %d",queue_size(PREPARADOS));
			if(queue_size(DISPONIBLES)>0 && queue_size(POKEMONS)>0){
				sem_post(&rellenar_colas);
			}else{
				sem_post(&s_ejecucion);
			}
		}
	}
	pthread_exit(NULL);
}

// HILO PLANIFICAR ENTRENADORES (INTERCAMBIO), EXISTE MIENTRAS COLA DEADLOCKS > 0
void match_intercambiar(){
	while(1){

	}
}

/*
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 */

float distancia(int e_x,int e_y,int p_x,int p_y){ // se calcula la distancia entre el pokemon recien aparecido y el entrenador en su posicion actual
	int distx= p_x - e_x;
	int disty= p_y - e_y;
	if(distx <0){distx = distx * (-1);}
	if(disty <0){disty = disty * (-1);}
	if(disty==0){return ((float)distx);}
	if(distx==0){return ((float)disty);}
	else{
		float distancia=0.0;
		float suma = (float) ((distx*distx) + (disty*disty));
		while((distancia*distancia)<suma){
			distancia += 0.01;
		}
		return distancia;
	}
}

int loNecesito(char* pokemon){
	int necesito=0;
	for(int x=0;x<list_size(OBJETIVO_GLOBAL);x++){
		if(strcmp(pokemon,retornarNombreFantasia(list_get(OBJETIVO_GLOBAL,x)))==0){
			necesito=1;
		}
	}
	return necesito;
}

void agregar_pokemon_cola(char* pokemon, int x, int y){
	Pokemon* nuevo;
	nuevo = crearPokemon(pokemon,x,y);

	pthread_mutex_lock(&modificar_cola_pokemons);
	queue_push(POKEMONS,nuevo);
	pthread_mutex_unlock(&modificar_cola_pokemons);

	printf("\n%s agregado a cola de pokemons",nuevo->nombre);
}

void planificacion_fifo(){
	printf("\n cant objetivos globales:%d \n",list_size(OBJETIVO_GLOBAL));
	for(int x=0;x<list_size(OBJETIVO_GLOBAL);x++){
		printf("%s\n",retornarNombreFantasia(list_get(OBJETIVO_GLOBAL,x)));
	}
	POKEMONS = queue_create();
	pthread_mutex_init(&modificar_cola_pokemons,NULL);
	sem_init(&s_match,0,0);
	sem_init(&s_ejecucion,0,0);
	sem_init(&mas_pokemons,0,1);
	sem_init(&ya_termine,0,0);
	sem_init(&siguiente,0,0);
	sem_init(&rellenar_colas,0,1);
	//sem_init(&s_intercambio,0,0);
	pthread_t atrapar;
	//pthread_t intercambiar;
	pthread_t ejecucion;
	pthread_create(&atrapar,NULL,(void*)match_atrapar,NULL);
	//pthread_create(&intercambiar,NULL,(void*)match_intercambiar,NULL);
	pthread_create(&ejecucion,NULL,(void*)ejecucion_entrenadores,NULL);

	//pthread_join(intercambiar,NULL);
	pthread_join(atrapar,NULL);
	pthread_join(ejecucion,NULL);
}
