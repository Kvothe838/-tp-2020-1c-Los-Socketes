#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include "../shared/structs.h"

void* serializar_paquete(Paquete* paquete, int *bytes);

void* serializarSuscripcion(Suscripcion* suscripto, int tamanio, void* stream);

#endif
