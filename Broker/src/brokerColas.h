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
  long ID;
  TipoCola cola;
  t_list* suscriptoresRecibidos; //ACK
  t_list* suscriptoresEnviados;
} MensajeEnCache;

typedef struct {
  long ID;
  long IDCorrelativo;
  void* contenido;
} MensajeEnCola;

/*CACHÉ:

  Identificador único del mensaje dentro del sistema.
  El tipo de mensaje (a que cola de mensajes pertenece).
  Los suscriptores a los cuales ya se envió el mensaje.
  Los suscriptores que retornaron el ACK del mismo.

*/

void crearDiccionario();
ColaConSuscriptores* crearColaConSuscriptores();
ColaConSuscriptores* obtenerCola(TipoCola colaClave);
void agregarSuscriptor(TipoCola colaClave, int nuevoSuscriptor);
void agregarMensaje(TipoCola colaClave, MensajeEnCola* mensaje);
t_list* obtenerSuscriptoresPorCola(TipoCola colaClave);

#endif /* BROKERCOLAS_H_ */
