#ifndef MESSAGES_H_
#define MESSAGES_H_

#include <stdarg.h>
#include "../shared/structs.h"

void mandarSuscripcion(int socket_server, int cantidadColasASuscribir, ...);
int enviarMensaje(void* mensaje, int tamanioMensaje, OpCode codMensaje, TipoCola colaMensaje, int socket_cliente);
void* recibirMensaje(int socket_cliente);
void* recibirMensajeServidor(int socket_cliente, int* size);
char* tipoColaToString(TipoCola tipoCola);
#endif
