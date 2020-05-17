#ifndef MESSAGES_H_
#define MESSAGES_H_

#include <stdarg.h>
#include "../shared/structs.h"

<<<<<<< HEAD
void mandarSuscripcion(int cantidadDeColasASuscribir, TipoCola colas[], int socket_server);
int enviar_mensaje(void* mensaje, int tamanioMensaje, OpCode codMensaje, int socket_cliente);
void* recibir_mensaje(int socket_cliente);
void* recibir_mensaje_servidor(int socket_cliente, int* size);

=======
void mandarSuscripcion(int socket_server, int cantidadColasASuscribir, ...);
int enviarMensaje(void* mensaje, int tamanioMensaje, OpCode codMensaje, int socket_cliente);
void* recibirMensaje(int socket_cliente);
void* recibirMensajeServidor(int socket_cliente, int* size);
char* tipoColaToString(TipoCola tipoCola);
>>>>>>> 600a0349be88af999d119a2d4215ef0e9f183d4e
#endif
