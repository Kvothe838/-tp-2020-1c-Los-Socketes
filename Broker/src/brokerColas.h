#ifndef BROKERCOLAS_H_
#define BROKERCOLAS_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <shared/utils.h>
#include "cache/dynamicCache.h"

typedef struct {
	t_list* suscriptores;
	t_list* IDMensajes;
} ColaConSuscriptores;

//sem_t* semColaMensajes;

void crearDiccionario();
void liberarDiccionario();
ColaConSuscriptores* crearColaConSuscriptores();
ColaConSuscriptores* obtenerCola(TipoCola colaClave);
/*void eliminarMensajeDeLista(int IDMensaje, TipoCola colaClave);*/

/*
 * Devuelve 1 si fue agregado exitosamente, o 0 si ya existía y su socket fue actualizado
*/
int agregarSuscriptor(TipoCola colaClave, Suscriptor* nuevoSuscriptor);
void agregarMensaje(TipoCola colaClave, long *IDMensaje);
t_list* obtenerSuscriptoresPorCola(TipoCola colaClave);

#endif /* BROKERCOLAS_H_ */
