#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "../shared/structs.h"
#include "../shared/messages.h"

pthread_t thread;

int crear_conexion_cliente(char *ip, char* puerto);
void liberar_conexion_cliente(int socket_cliente);

#endif
