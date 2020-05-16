#ifndef TEAMINIT_H_
#define TEAMINIT_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include <shared/utils.h>

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

typedef Entrenador** Team;

Entrenador* inicializarEntrenador(int id,char*posicion ,char* pokePertenecientes, char* pokeObjetivos);

#endif /* TEAMINIT_H_ */
