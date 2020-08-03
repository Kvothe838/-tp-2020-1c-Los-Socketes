#ifndef TEAMINIT_H_
#define TEAMINIT_H_


#include <stdlib.h>
#include <stdio.h>

#include <commons/collections/list.h>
#include <pthread.h>
#include <shared/utils.h>
#include <semaphore.h>
#include <commons/collections/queue.h>

t_log* logger;
FILE* logTP;
FILE* metricasTP;
int CANT_ENTRENADORES;
int CAMBIOS_CONTEXTO;
int DEADLOCKS_RESUELTOS;
//int MENSAJES_BROKER;

t_list* OBJETIVO_GLOBAL;
t_list* OBJETIVO_GLOBAL_FILTRADO;
t_list* METRICAS_ENTRENADORES;

t_queue* PREPARADOS;
t_queue* DISPONIBLES;
t_queue* POKEMONS;
t_queue* DEADLOCKS;
t_queue* EJECUTADOS;
t_queue* ESPERANDO;

sem_t s_match;
sem_t s_ejecucion;
sem_t s_intercambio;
sem_t progreso;
sem_t mas_pokemons;
sem_t ya_termine;
sem_t siguiente;
sem_t rellenar_colas;
sem_t esperarPokemons;
sem_t esperar_pokemons;
sem_t esperar_finalizacion;
sem_t finalizar_ejecucion;
sem_t intercambio_hecho;
sem_t hayPreparados;

pthread_mutex_t modificar_cola_preparados;
pthread_mutex_t modificar_cola_disponibles;
pthread_mutex_t modificar_cola_pokemons;
pthread_mutex_t modificar_cola_deadlocks;
pthread_mutex_t modificar_cola_ejecutados;
pthread_mutex_t modificar_cola_esperando;

typedef enum {
	ATRAPAR,
	INTERCAMBIAR,
}TipoAccion;

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
	t_list* pertenecientesIntercambiables;
	sem_t activador;
	Pokemon* intentar_atrapar;
	TipoAccion tipoAccion;
	int indiceDar;
	int quantum_restante;
}Entrenador;

typedef struct{
	char** posiciones;
	char** pertenecientes;
	char** objetivos;
	char* ip;
	char* puerto;
	char* ipTeam;
	char* puertoTeam;
	char* planificacion;
	int quantum;
	int tiempoReconexion;
	int retardo_cpu;
	char* path;
	int ID;
}Config;

typedef struct{
	int idEntrenador;
	int quantumTotal;
}Metrica;

typedef struct{
	char* especie;
	int cantidad;
	int aceptarMas;
}Objetivo;

typedef Entrenador** Team;
//Team team;
Config* configTeam;

// RETORNOS
char* retornarNombrePosta(Pokemon* p);
int get_id(Entrenador* persona);
char* retornarNombreFantasia(PokemonFantasia* p);
int get_posicion(Entrenador* persona,int eje);
void mostrarEntrenador(Entrenador* entrenador);
void mostarObjetivosActualesDe(Entrenador* entrenador);
int cant_entrenadores(char** posiciones);
void getObjetivosGlobales(Team team);
int verificar_deadlock(Entrenador* persona);
void mostrarPertenecientesIntercambiables(Entrenador* entrenador);
Entrenador* getEntrenador(Entrenador* entrenador);
void mostrarColas();
Objetivo* getObj(Objetivo* x);
int retornarPosicion(Pokemon* p,int eje);


// INICIALIZAR Y CONFIGURAR
void cargarConfig(Config* conexionConfig, t_log* logger);
void iniciar_entrenador(Entrenador** entrenador);
Entrenador* inicializarEntrenador(int id,char*posicion ,char* pokePertenecientes, char* pokeObjetivos);
Entrenador** inicializarTeam(char** posiciones, char** pokePertenecientes , char** pokeObjetivos);
void asignarPosicion(Entrenador* persona,char* posicion);
void asignarPertenecientes(Entrenador* persona,char* pokemons);
void asignarObjetivos(Entrenador* persona,char* pokemons);
void asignarObjetivosActuales(Entrenador* persona);
void asignarPertenecientesIntercambiables(Entrenador* persona);
void actualizar_estado(Entrenador* persona);
void ingreso_a_colas_entrenador(Entrenador* persona);

// CREACIONES
Pokemon* crearPokemon(char* nombre,int x, int y);
PokemonFantasia* crearObjetivo(char* nombre);

void sumarQuantum(int indice, int cantSumar);
int getQuantum(int indice);
Metrica* getMetrica(int indice);

// EVITAR MEMORY LEAKS
void liberarTeam(Team team);

#endif /* TEAMINIT_H_ */
