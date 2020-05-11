#include "../shared/serialize.h"
//serializar_get
void* serializar_paquete(t_paquete* paquete, int *bytes)
{
	*bytes = sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;
	void* a_enviar = malloc(*bytes);
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(paquete->codigo_operacion));
	offset += sizeof(paquete->codigo_operacion);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	offset += sizeof(paquete->buffer->size);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;

	return a_enviar;
}

//Acá hay que hacer una función para cada estructura de cola que se quiera serializar (6 estructuras).
