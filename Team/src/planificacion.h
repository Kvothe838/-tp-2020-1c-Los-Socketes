#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <stdlib.h>
#include <stdio.h>

#include "teamInit.h"
//#include "conexionesTeam.h"



int get_id(Entrenador* persona);
void match_atrapar();
void asignar_movimiento(Entrenador* entrenador,int mov_x,int mov_y,Pokemon* pokemon);
float distancia(int e_x,int e_y,int p_x,int p_y);
void entrenador_mas_cercano(Pokemon* pokemon);
void matchear_pokemon();
int loNecesito(char* pokemon);
void agregar_pokemon_cola(char* pokemon, int x, int y);
void llegada_pokemons();
void planificacion_fifo();

#endif /* PLANIFICACION_H_ */
