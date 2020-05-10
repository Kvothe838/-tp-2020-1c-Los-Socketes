#ifndef MESSAGES_H_
#define MESSAGES_H_

#include "../shared/structs.h"

int enviar_mensaje(int* mensaje, int tamanioMensaje, op_code codMensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
void* recibir_mensaje_servidor(int socket_cliente, int* size);

#endif
