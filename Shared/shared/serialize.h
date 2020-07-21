#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include "../shared/structs.h"
#include <stdarg.h>
#include <commons/collections/list.h>

void* armarPaqueteYSerializar(OpCode codigoOperacion, int tamanioContenido, void* contenido, int* bytes);
void* armarYSerializarAck(long IDMensaje, TipoModulo modulo, int* bytes);
void* serializarSuscripcion(int cantidadColas, TipoCola* colas, int* tamanio);
void* armarYSerializarContenidoHaciaBroker(TipoModulo modulo, int tamanioContenido, void* contenido, int* bytes);
void* serializarPublisher(TipoModulo modulo, TipoCola cola, void* dato, long IDCorrelativo, int* bytes);
void* armarYSerializarPublicacion(TipoCola cola, long IDCorrelativo, void* dato, int bytesDato, int* bytes);

void* serializarDato(void* mensaje, int* tamanio, TipoCola cola);
void* serializarNew(NewPokemon* pokemon, int* bytes);
void* serializarAppeared(AppearedPokemon* pokemon, int* bytes);
void* serializarCatch(CatchPokemon* pokemon, int* bytes);
void* serializarCaught(CaughtPokemon* pokemon, int* bytes);
void* serializarGet(GetPokemon* pokemon, int* bytes);
NewPokemon* deserializarNew(void* mensaje);
AppearedPokemon* deserializarAppeared(void* mensaje);
CaughtPokemon* deserializarCaught(void* mensajes);
CatchPokemon* deserializarCatch(void* mensaje);
GetPokemon* deserializarGet(void* mensaje);
void* deserializarDato(void* mensaje, TipoCola cola);

void* serializarStreamIdMensajePublisher(long ID, TipoCola cola, int* bytes);
void* serializarMensajeSuscriptor(long ID, long IDCorrelativo, void* contenido, int tamanioContenido, TipoCola tipoCola, int* bytes);


#endif
