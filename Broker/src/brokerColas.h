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
	t_list* mensajes;
	//t_queue* colaMensajes;
} ColaConSuscriptores;

typedef struct {
	int ID;
	int IDCorrelativo;
	t_list* suscriptoresRecibidos;
	t_list* suscriptoresEnviados;
} Mensaje;

void crearDiccionario();
ColaConSuscriptores* crearColaConSuscriptores();
ColaConSuscriptores* obtenerCola(TipoCola colaClave);
void agregarSuscriptor(TipoCola colaClave, int nuevoSuscriptor);
void agregarMensaje(TipoCola colaClave, void* mensaje);
void obtenerSuscriptoresPorCola(TipoCola colaClave);

#endif /* BROKERCOLAS_H_ */
