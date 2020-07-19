#ifndef TEAMINIT_H_
#define TEAMINIT_H_


#include <stdlib.h>
#include <stdio.h>

#include <commons/collections/list.h>
#include <pthread.h>
#include <shared/utils.h>
#include <semaphore.h>
#include <commons/collections/queue.h>


t_list* OBJETIVO_GLOBAL;
t_queue* PREPARADOS;
t_queue* DISPONIBLES;
t_queue* POKEMONS;
t_queue* DEADLOCKS;

sem_t s_match;
sem_t s_ejecucion;
sem_t s_intercambio;
sem_t progreso;
sem_t mas_pokemons;
sem_t ya_termine;
sem_t siguiente;
sem_t rellenar_colas;

pthread_mutex_t modificar_cola_preparados;
pthread_mutex_t modificar_cola_disponibles;
pthread_mutex_t modificar_cola_pokemons;
pthread_mutex_t modificar_cola_deadlocks;

typedef enum {
	NUEVO,
	LISTO,
	EJECUTANDO,
	BLOQUEADO,
	SALIR,
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
	int idEntrenador;
	int posicion[2];
	int movimiento[2];
	t_list* mios;
	Estado estado;
	t_list* objetivos;
	t_list* objetivosActuales;
	sem_t activador;
	Pokemon* intentar_atrapar;
}Entrenador;

typedef struct{
	char** posiciones;
	char** pertenecientes;
	char** objetivos;
	char* ip;
	char* puerto;
	int ID;
}Config;
typedef Entrenador** Team;

// RETORNOS
char* retornarNombrePosta(Pokemon* p);
char* retornarNombreFantasia(PokemonFantasia* p);
int get_posicion(Entrenador* persona,int eje);
void mostrarEntrenador(Entrenador* entrenador);
void mostarObjetivosActualesDe(Entrenador* entrenador);
int cant_entrenadores(char** posiciones);
Entrenador* getEntrenador(int id,Team team);
void getObjetivosGLobales(Team team);
int verificar_deadlock(Entrenador* persona);


// INICIALIZAR Y CONFIGURAR
void cargarConfig(Config* conexionConfig, t_log* logger);
void iniciar_entrenador(Entrenador** entrenador);
Entrenador* inicializarEntrenador(int id,char*posicion ,char* pokePertenecientes, char* pokeObjetivos);
Entrenador** inicializarTeam(char** posiciones, char** pokePertenecientes , char** pokeObjetivos);
void asignarPosicion(Entrenador* persona,char* posicion);
void asignarPertenecientes(Entrenador* persona,char* pokemons);
void asignarObjetivos(Entrenador* persona,char* pokemons);
void asignarObjetivosActuales(Entrenador* persona);
void actualizar_estado(Entrenador* persona);
void ingreso_a_colas_entrenador(Entrenador* persona);

// CREACIONES
Pokemon* crearPokemon(char* nombre,int x, int y);
PokemonFantasia* crearObjetivo(char* nombre);




// EVITAR MEMORY LEAKS
void liberarTeam(Team team);
void liberarMemoria(Team team);

#endif /* TEAMINIT_H_ */
