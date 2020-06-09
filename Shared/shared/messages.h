#ifndef MESSAGES_H_
#define MESSAGES_H_

#include <stdarg.h>
#include "../shared/structs.h"

void mandarSuscripcion(int socket_server, int cantidadColasASuscribir, ...);
int enviarMensaje(void* mensaje, int tamanioMensaje, OpCode codMensaje, TipoCola colaMensaje, int socket_cliente);
void* recibirMensaje(int socket_cliente);
void* recibirMensajeServidor(int socket_cliente, int* size);
char* tipoColaToString(TipoCola tipoCola);
TipoModulo argvToTipoModulo(char* modulo);
TipoCola argvToTipoCola(char* cola);
NewPokemon* getNewPokemon(char* nombre, int posX, int posY, int cantidad);
AppearedPokemon* getAppearedPokemon(char* nombre, int posX, int posY);
CatchPokemon* getCatchPokemon(char* nombre, int posX, int posY);
CaughtPokemon* getCaughtPokemon(int loAtrapo);
GetPokemon* getGetPokemon(char* nombre);
uint32_t enviarMensajeASuscriptor(uint32_t socket_suscriptor, long IDCorrelativo, TipoCola colaDeSalida, void* data, uint32_t tamanioData);
#endif
