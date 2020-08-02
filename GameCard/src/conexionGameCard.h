#ifndef CONEXIONBROKER_H_
#define CONEXIONBROKER_H_

#include <shared/structs.h>
#include <shared/messages.h>
#include <shared/serialize.h>

pthread_t thread;
pthread_t threadGameBoy;

char* ipBroker, *puertoBroker;

void escucharBroker();
void iniciarServidorGameboy(IniciarServidorArgs* argumentos);
void iniciar_servidor(t_config* config, int socketBroker);
#endif
