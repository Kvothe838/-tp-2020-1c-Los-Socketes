#ifndef CONEXIONESTEAM_H_
#define CONEXIONESTEAM_H_

#include <stdlib.h>
#include <stdio.h>

#include "teamInit.h"

pthread_t threadMensajeGameBoy;

int enviarGet(char* nombre);
int loNecesitoGlobalmenteAppeared(char* nombrePokemon);
int loNecesitoGlobalmente(char* nombrePokemon);
float distanciaMinimaHaciaAlgunEntrenador(Pokemon* pokemon);
void reservarPokemon(Pokemon* pokemon);
void filtrarPokemonsLocalized(t_list* pokemonsLocalized, int cantidadNecesidadGlobal);
void manejarNuevoMensajeSuscriptor(MensajeParaSuscriptor* mensaje);
void enviarTodosLosGets();
void escucharABroker();
void conectarse_broker();
void atenderMensaje(int* socket);
void esperarConexionGameBoy(int socket);
void iniciarServidorTeam();
void conexiones(Entrenador** team);

#endif /* CONEXIONESTEAM_H_ */
