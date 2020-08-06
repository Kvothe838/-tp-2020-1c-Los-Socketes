#ifndef CONEXIONESTEAM_H_
#define CONEXIONESTEAM_H_

#include <stdlib.h>
#include <stdio.h>

#include "teamInit.h"

pthread_t threadMensajeGameBoy;
char* ipBroker, *puertoBroker;

void conexiones(t_log* logger, Entrenador** team);
void iniciarServidorTeam();

#endif /* CONEXIONESTEAM_H_ */
