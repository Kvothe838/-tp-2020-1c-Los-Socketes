#ifndef MESSAGES_H_
#define MESSAGES_H_

#include "../shared/structs.h"

void mandarSuscripcion(int cantidadDeColasASuscribir, TipoCola colas[], int socket_server);
int enviar_mensaje(void* mensaje, int tamanioMensaje, OpCode codMensaje, int socket_cliente);
void* recibir_mensaje(int socket_cliente);
void* recibir_mensaje_servidor(int socket_cliente, int* size);

#endif
