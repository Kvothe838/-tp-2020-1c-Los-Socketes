#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include "../shared/structs.h"

void* serializarPaquete(Paquete* paquete, int *bytes);
Paquete* armarPaquete(OpCode codigoOperacion, int tamanio, void* stream);
void* armarPaqueteYSerializar(OpCode codigoOperacion, int tamanio, void* stream, int* bytes);
void* serializarSuscripcion(Suscripcion* suscripcion, int tamanio);
void* serializarDato(void* mensaje, int tamanioMensaje, TipoCola colaMensaje);
void* serializarNew(NewPokemon* pokemon, int* bytes, TipoCola colaMensaje);
void* serializarAppeared(AppearedPokemon* pokemon, int* bytes, TipoCola colaMensaje);
void* serializarCatch(CatchPokemon* pokemon, int* bytes, TipoCola colaMensaje);
void* serializarCaught(CaughtPokemon* pokemon, int* bytes, TipoCola colaMensaje);
void* serializarGet(GetPokemon* pokemon, int* bytes, TipoCola colaMensaje);
void* serializarStreamIdMensajePublisher(long ID, TipoCola cola);
void* serializarMensajeSuscriptor(MensajeEnCola mensajeEnCola, TipoCola tipoCola);

void* deseralizarDato(void* mensaje,int* tamanioMensaje);


#endif
