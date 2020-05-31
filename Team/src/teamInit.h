#ifndef TEAMINIT_H_
#define TEAMINIT_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include <shared/utils.h>

t_list* OBJETIVO_GLOBAL;
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
	int idEntrenador;
	int posicion[2];
	t_list* mios;
	Estado estado;
	t_list* objetivos;
	t_list* objetivosActuales;
}Entrenador;

typedef struct{
	char** posiciones;
	char** pertenecientes;
	char** objetivos;
	char* ip;
	char* puerto;
}Config;


typedef Entrenador** Team;

//carga el configTeam
void cargarConfig(Config* conexionConfig);

//inizializar
Pokemon* crearPokemon(char* nombre,int x, int y);
PokemonFantasia* crearObjetivo(char* nombre);
int cant_entrenadores(char** posiciones);
void asignarPosicion(Entrenador* persona,char* posicion);
void asignarPertenecientes(Entrenador* persona,char* pokemons);
void asignarObjetivos(Entrenador* persona,char* pokemons);
void inicializar_entrenadores(int indice, Entrenador entrenador,char* posicion,char* pertenecientes,char* objetivos);
Entrenador* inicializarEntrenador(int id,char*posicion ,char* pokePertenecientes, char* pokeObjetivos);
Entrenador** inicializarTeam(char** posiciones, char** pokePertenecientes , char** pokeObjetivos);
void asignarObjetivosActuales(Entrenador* persona);
void mostarObjetivosActualesDe(Entrenador* entrenador);
void mostrarEntrenador(Entrenador* entrenador);
Entrenador* getEntrenador(int id,Team team);
void getObjetivosGLobales(Team team);
//cambiar el nombre

char* retornarNombrePosta(Pokemon* p);
char* retornarNombreFantasia(PokemonFantasia* p);

// funciones para evitar Memory Leask
void liberarTeam(Team team);
void liberarMemoria(Team team);

#endif /* TEAMINIT_H_ */
