#ifndef MESSAGES_H_
#define MESSAGES_H_

#include <stdarg.h>
#include "../shared/structs.h"

void mandarSuscripcion(int socket_server, TipoModulo modulo, int cantidadColasASuscribir, ...);
int enviarMensajeASuscriptor(int socketSuscriptor, long ID, long* IDCorrelativo, TipoCola cola, void* data);
void* recibirMensaje(int socket_cliente);
int enviarMensaje(void* mensaje, OpCode codigoOperacion, TipoCola cola, long* IDCorrelativo, int socket_cliente,
		TipoModulo modulo);

/**
 * Dado un Buffer con size y stream enviado desde socket_cliente, devuelve stream y asigna a size.
 *
 * @socketCliente El socket del cliente que envió el paquete
 * @size Un puntero a una variable vacía para almacenar el tamaño del stream
 * @return El stream dentro del paquete en forma de void*
 */
void* recibirMensajeServidor(int socketCliente, int* size);
char* tipoColaToString(TipoCola tipoCola);
TipoModulo argvToTipoModulo(char* modulo);
TipoCola argvToTipoCola(char* cola);
NewPokemon* getNewPokemon(char* nombre, int posX, int posY, int cantidad);
AppearedPokemon* getAppearedPokemon(char* nombre, int posX, int posY);
CatchPokemon* getCatchPokemon(char* nombre, int posX, int posY);
CaughtPokemon* getCaughtPokemon(int loAtrapo);
GetPokemon* getGetPokemon(char* nombre);

#endif
