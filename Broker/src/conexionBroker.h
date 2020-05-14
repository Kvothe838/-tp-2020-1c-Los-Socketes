#ifndef CONEXIONBROKER_H_
#define CONEXIONBROKER_H_

#include <shared/structs.h>
#include <shared/messages.h>
#include "brokerColas.h"

pthread_t thread;

void iniciar_servidor(char *ip, char* puerto);

#endif
