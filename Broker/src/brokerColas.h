#ifndef BROKERCOLAS_H_
#define BROKERCOLAS_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <shared/utils.h>

typedef struct {
	t_list* suscriptores;
	t_list* IDMensajes;
} ColaConSuscriptores;

void crearDiccionario();
ColaConSuscriptores* crearColaConSuscriptores();
ColaConSuscriptores* obtenerCola(TipoCola colaClave);
void agregarSuscriptor(TipoCola colaClave, Suscriptor nuevoSuscriptor);
void agregarMensaje(TipoCola colaClave, int IDMensaje);
t_list* obtenerSuscriptoresPorCola(TipoCola colaClave);

#endif /* BROKERCOLAS_H_ */
