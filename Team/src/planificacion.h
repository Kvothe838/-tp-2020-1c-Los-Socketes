#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <stdlib.h>
#include <stdio.h>

#include "teamInit.h"

void asignar_movimiento(Entrenador* entrenador,int mov_x,int mov_y);
float distancia(int e_x,int e_y,int p_x,int p_y);
void cercania(Team team,int p_x, int p_y);
void planificacion_fifo(Team team);

#endif /* PLANIFICACION_H_ */
