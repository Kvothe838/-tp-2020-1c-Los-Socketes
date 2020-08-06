#ifndef CONEXIONESTEAM_H_
#define CONEXIONESTEAM_H_

#include <stdlib.h>
#include <stdio.h>

#include "teamInit.h"

pthread_t threadMensajeGameBoy;
char* ipBroker, *puertoBroker;

void conexiones(Entrenador** team);
void iniciarServidorTeam();

#endif /* CONEXIONESTEAM_H_ */
