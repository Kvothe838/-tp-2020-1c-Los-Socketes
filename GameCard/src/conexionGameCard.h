#ifndef CONEXION_GAME_CARD_H_
#define CONEXION_GAME_CARD_H_

#include <shared/structs.h>
#include <shared/messages.h>

pthread_t thread;

void esperarCliente(int socket_servidor);

#endif
