#ifndef CONEXIONBROKER_H_
#define CONEXIONBROKER_H_

#include <shared/structs.h>
#include <shared/messages.h>

pthread_t thread;

void iniciar_servidor(char *ip, char* puerto);

#endif
