#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/node.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

#include <shared/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef enum estado{
	NUEVO = 1,
	LISTO,
	EJECUTANDO,
	BLOQUEADO,
	SALIR,
	PUNTOMUERTO
};
typedef struct entrenador{
	int ID_entrenador;
	t_list* posicion; // X = list_get(entrenador.posicion,0) // Y = list_get(entrenador.posicion,1)
	t_list* mios;
	t_list* objetivos;
	enum estado;
	// UN HILO ASOCIADO (?)
}; // nose para que pongo este, pero compila

int cant_entrenadores(char** posiciones){
	int cantidad = 0;
	int i=0;
	while(posiciones[i] != NULL){
		cantidad++;
		i++;
	}
	return cantidad;
}
void separarElementosNumericos(t_list* lista,char* posicion){
	char* token = strtok(posicion, "|");
	while (token != NULL) {
		list_add(lista,atoi(token));
	    token = strtok(NULL, "|");
	}
}
void separarElementos(t_list* lista,char* pokemons){
	char* token = strtok(pokemons, "|");
	while (token != NULL) {
		list_add(lista,token);
	    token = strtok(NULL, "|");
	}
}

void inicializar_entrenadores(int indice, struct entrenador entrenador,char* posicion,char* pertenecientes,char* objetivos){
	printf("hola, soy el entrenador nro %d \n",indice);
	entrenador.ID_entrenador = indice+1;
	entrenador.posicion = list_create();separarElementosNumericos(entrenador.posicion,posicion);
	entrenador.objetivos = list_create();separarElementos(entrenador.objetivos,objetivos);
	entrenador.mios = list_create();separarElementos(entrenador.mios,pertenecientes);
	//printf("x: %d ---- y: %d \n",list_get(entrenador.posicion,0),list_get(entrenador.posicion,1));
	//printf("p1: %s \n",list_get(entrenador.mios,0));
	printf("cant: %d \n",list_size(entrenador.mios));
}

int main(void) {
	t_log* logger;
	t_config* config;
	logger = iniciar_logger("Team.log", "Team");
	config = leer_config("configTeam.config", logger);
	char** posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES"); // lista de strings, ultimo elemento nulo
	char** pertenecientes = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

	int cantidad_entrenadores = cant_entrenadores(posiciones);
	//printf("cantidad entrenadores en el archivo: %d \n",cantidad_entrenadores);

	struct entrenador entrenadores[cantidad_entrenadores];

	for(int a=0;a<cantidad_entrenadores;a++){
		inicializar_entrenadores(a,entrenadores[a],posiciones[a],pertenecientes[a],objetivos[a]);
	}


	//pthread_t hilo[cantidad_entrenadores];
	//pthread_create(&hilo[b], NULL, &inicializar_entrenador, NULL);
/*
	for(int c=0;c<cantidad_entrenadores;c++){
		printf("se murio un hilo \n");
		pthread_join(&hilo[c],NULL);
	}
*/
	printf(" \n programa finalizado");
}
