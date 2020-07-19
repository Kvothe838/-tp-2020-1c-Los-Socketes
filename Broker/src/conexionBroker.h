#ifndef CONEXIONBROKER_H_
#define CONEXIONBROKER_H_

#include <shared/structs.h>
#include <shared/messages.h>
#include <shared/serialize.h>
#include "brokerColas.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/temporal.h>
#include <string.h>
#include <pthread.h>
#include <shared/connection.h>
#include <shared/commonsExtensions.h>

pthread_t thread;
t_log* logger;

typedef struct {
	char* ip;
	char* puerto;
} IniciarServidorArgs;

void iniciarServidor(IniciarServidorArgs* argumentos);
void enviarMensajesSuscriptores();

#endif
