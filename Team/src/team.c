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
typedef enum {
	NUEVO,
	LISTO,
	EJECUTANDO,
	BLOQUEADO,
	SALIR,
	PUNTOMUERTO
}Estado;

typedef struct {
    char* nombre;
    int x;
    int y;
} Pokemon;
typedef struct {
    char* nombre;
} PokemonFantasia;
typedef struct {
	int ID_entrenador;
	int posicion[2];
	t_list* mios;
	Estado estado;
	int maximosPokemons;
	int cantPokemons;
	t_list* objetivos;
}Entrenador;


static Pokemon *crearPokemon(char *nombre,int x, int y) {
	Pokemon *new = malloc(sizeof(Pokemon));
    new->nombre = strdup(nombre);
    new->y=y;
    new->x=x;
    return new;
}
static PokemonFantasia *crearObjetivo(char *nombre) {
	PokemonFantasia *new = malloc(sizeof(PokemonFantasia));
    new->nombre = strdup(nombre);
    return new;
}


int cant_entrenadores(char** posiciones){
	int cantidad = 0;
	int i=0;
	while(posiciones[i] != NULL){
		cantidad++;
		i++;
	}
	return cantidad;
}
void asignarPosicion(Entrenador* persona,char* posicion){
	char* token = strtok(posicion, "|");
	int eje=0;
	while (token != NULL) {
		persona->posicion[eje] = atoi(token);
	    token = strtok(NULL, "|");
	    eje++;
	}
}
void asignarPertenecientes(Entrenador* persona,char* pokemons){
	char* token = strtok(pokemons, "|");
	persona->mios = list_create();
	while (token != NULL) {
		Pokemon* p;
		p = crearPokemon(token,0,0);
		list_add(persona->mios,p);
		token = strtok(NULL, "|");
	}
}
void asignarObjetivos(Entrenador* persona,char* pokemons){
	char* token = strtok(pokemons, "|");
		persona->objetivos = list_create();
		while (token != NULL) {
			PokemonFantasia* p;
			p = crearObjetivo(token);
			list_add(persona->objetivos,p);
			token = strtok(NULL, "|");
		}
}
void inicializar_entrenadores(int indice, Entrenador entrenador,char* posicion,char* pertenecientes,char* objetivos){
	asignarPosicion(&entrenador,posicion);
	asignarPertenecientes(&entrenador,pertenecientes);
	asignarObjetivos(&entrenador,objetivos);
	//printf("x: %d y: %d \n ",entrenador.posicion[0],entrenador.posicion[1]);
	printf("xd: %d",list_size(entrenador.objetivos));
}

Entrenador* inicializarEntrenador(int id,char* posicion, char* pokePertenecientes , char* pokeObjetivos){
	Entrenador* entrenador = (Entrenador*)malloc(sizeof(Entrenador));

	entrenador->ID_entrenador = id;
	asignarPosicion(entrenador,posicion);
	asignarPertenecientes(entrenador,pokePertenecientes);
	asignarObjetivos(entrenador,pokeObjetivos);
	entrenador->estado = NUEVO;
	entrenador->maximosPokemons = sizeof(entrenador->objetivos)- 1;
	entrenador->cantPokemons = sizeof(entrenador->mios)- 1;

	return entrenador;

}
Entrenador** inicializarTeam(char** posiciones, char** pokePertenecientes , char** pokeObjetivos){
	Entrenador** team = (Entrenador**)(malloc(sizeof(Entrenador)));
	int cant = sizeof(posiciones) - 1;
	for(int i=0 ; i<cant ; i++){
		team[i] = inicializarEntrenador(i,posiciones[i],pokePertenecientes[i],pokeObjetivos[i]);
	}
	return team;
}

static void liberarTeam(Entrenador **team){
	for(int i=0 ; i< sizeof(team)-1 ;i++){
		free(team[i]);
	}
	free(team);
}
void liberarMemoria(Entrenador **team){
	liberarTeam(team);
}


int main(void) {
	t_log* logger;
	t_config* config;
	logger = iniciar_logger("Team.log", "Team");
	config = leer_config("configTeam.config", logger);
	char** posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES"); // lista de strings, ultimo elemento nulo
	char** pertenecientes = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");


	//printf("cantidad entrenadores en el archivo: %d \n",cantidad_entrenadores);

	//struct entrenador entrenadores[cantidad_entrenadores];
	//crear una lista de entrenadores
	Entrenador** team = inicializarTeam(posiciones,pertenecientes,objetivos);

	printf("\n entrenador inicial id %d", team[1]->ID_entrenador);


	//t_list* list_filter(t_list*, bool(*condition)(void*));
	//void *list_remove_by_condition(t_list *, bool(*condition)(void*));
	//void *list_find(t_list *, bool(*closure)(void*));
	//int list_size(t_list *);


	//pthread_t hilo[cantidad_entrenadores];
	//pthread_create(&hilo[b], NULL, &inicializar_entrenador, NULL);
/*
	for(int c=0;c<cantidad_entrenadores;c++){
		printf("se murio un hilo \n");
		pthread_join(&hilo[c],NULL);
	}
*/
	/*t_pokemon* p1;
	p1 = pokemon_create("gabo");
	t_pokemon* p2;
	p2 = pokemon_create("samuel");
	t_list* lista = list_create();
	list_add(lista,p1);
	list_add(lista,p2);
	printf("x: %d",list_size(lista));
	t_list* nombres = list_create();
	nombres = list_map(lista,obtenerNombre);
	printf("             y: %s",list_get(nombres,0));*/
	liberarMemoria(team);
	printf(" \n programa finalizado");
}















