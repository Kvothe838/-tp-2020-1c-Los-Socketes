#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include "../shared/structs.h"

void* serializar_paquete(Paquete* paquete, int *bytes);

void* serializarSuscripcion(Suscripcion* suscripto, int tamanio, void* stream);

void* serializarDato(void* mensaje, int tamanioMensaje, TipoCola colaMensaje);

void* serializarNew(NewPokemon* pokemon, int* bytes, TipoCola colaMensaje);

void* serializarAppeared(AppearedPokemon* pokemon, int* bytes, TipoCola colaMensaje);

void* serializarCatch(CatchPokemon* pokemon, int* bytes, TipoCola colaMensaje);

void* serializarCaught(CaughtPokemon* pokemon, int* bytes, TipoCola colaMensaje);

void* serializarGet(GetPokemon* pokemon, int* bytes, TipoCola colaMensaje);

#endif
