#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include "../shared/structs.h"

void* serializarPaquete(Paquete* paquete, int *bytes);
Paquete* armarPaquete(OpCode codigoOperacion, TipoModulo modulo, int tamanio, void* stream);
void* armarPaqueteYSerializar(OpCode codigoOperacion, TipoModulo modulo, int tamanio, void* stream, int* bytes);
void* serializarSuscripcion(Suscripcion* suscripcion, int* tamanio);
void* serializarDato(void* mensaje, int* tamanio, TipoCola cola);
void* serializarNew(NewPokemon* pokemon, int* bytes);
void* serializarAppeared(AppearedPokemon* pokemon, int* bytes);
void* serializarCatch(CatchPokemon* pokemon, int* bytes);
void* serializarCaught(CaughtPokemon* pokemon, int* bytes);
void* serializarGet(GetPokemon* pokemon, int* bytes);
void* serializarStreamIdMensajePublisher(long ID, TipoCola cola);
void* serializarMensajeSuscriptor(long ID, long* IDCorrelativo, void* contenido, int tamanioContenido, TipoCola tipoCola);

void* deseralizarDato(void* mensaje,int* tamanioMensaje);


#endif
