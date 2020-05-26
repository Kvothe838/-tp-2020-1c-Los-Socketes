#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include "../shared/structs.h"

void* serializar_paquete(Paquete* paquete, int *bytes);

void* serializarSuscripcion(Suscripcion* suscripto, int tamanio, void* stream);

void* serializarDato(void* mensaje, int* tamanioMensaje, void* stream, TipoCola colaMensaje);

void* serializarNew(NewPokemon* pokemon, int* bytes);

void* serializarAppeared(AppearedPokemon* pokemon, int* bytes);

void* serializarCatch(CatchPokemon* pokemon, int* bytes);

void* serializarCaught(CaughtPokemon* pokemon, int* bytes);

void* serializarGet(GetPokemon* pokemon, int* bytes);

#endif
