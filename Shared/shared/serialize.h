#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include "../shared/structs.h"

void* serializar_paquete(Paquete* paquete, int *bytes);

void* serializarNew(Paquete* paquete, int *bytes);

#endif
