#include "../shared/serialize.h"
//serializar_get
void* serializar_paquete(Paquete* paquete, int *bytes)
{
	*bytes = sizeof(paquete->codigoOperacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;
	void* a_enviar = malloc(*bytes);
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigoOperacion), sizeof(paquete->codigoOperacion));
	offset += sizeof(paquete->codigoOperacion);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	offset += sizeof(paquete->buffer->size);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;

	return a_enviar;
}

void* serializarSuscripcion(Suscripcion* suscripto, int tamanio, void* stream){
	stream = malloc(tamanio);
	void *prueba = malloc(tamanio);
	int ayuda, offset = 0, imprimir;
	memcpy(stream, &suscripto->cantidadColas, sizeof(int));

	memcpy(&ayuda, stream, sizeof(int));

	printf("CANTIDAD DE COLAS: %d\n", ayuda);

	offset += sizeof(TipoCola);
	memcpy(&ayuda, stream, sizeof(int));

	prueba = malloc(sizeof(int) * ayuda);
	memcpy(prueba, (suscripto->colas), sizeof(int) * ayuda);

	for(int i = 0; i < ayuda; i++){
		memcpy(stream + offset, (prueba + offset - sizeof(int)), sizeof(int));
		memcpy(&imprimir, stream + offset, sizeof(int));
		offset += sizeof(int);
	}

	return stream;
}

void* serializarDato(void* mensaje, int tamanioMensaje, void* stream, TipoCola colaMensaje){
	switch(colaMensaje){
		case NEW:
			return serializarNew(mensaje, &tamanioMensaje, colaMensaje);
			break;
		case APPEARED:
			return serializarAppeared(mensaje, &tamanioMensaje, colaMensaje);
			break;
		case CATCH:
			return serializarCatch(mensaje, &tamanioMensaje, colaMensaje);
			break;
		case CAUGHT:
			return serializarCaught(mensaje, &tamanioMensaje, colaMensaje);
			break;
		case GET:
			return serializarGet(mensaje, &tamanioMensaje, colaMensaje);
			break;
		default:
			break;
	}
}
//Acá hay que hacer una función para cada estructura de cola que se quiera serializar (6 estructuras).


void* serializarNew(NewPokemon* pokemon, int* bytes, TipoCola colaMensaje){

	void* stream =  malloc(*bytes);
	int offset = 0;

	memcpy(stream + offset,&colaMensaje, sizeof(TipoCola));
	offset += sizeof(TipoCola);

	memcpy(stream + offset, &pokemon->posX, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->posY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->cantidad, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->largoNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon->nombre, pokemon->largoNombre + 1);

	return stream;
}

void* serializarAppeared(AppearedPokemon* pokemon, int* bytes, TipoCola colaMensaje){

	void* stream =  malloc(*bytes);
	int offset = 0;

	memcpy(stream + offset,&colaMensaje, sizeof(TipoCola));
	offset += sizeof(TipoCola);

	memcpy(stream + offset, &pokemon->posX, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->posY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->largoNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon->nombre, pokemon->largoNombre + 1);

	return stream;
}

void* serializarCatch(CatchPokemon* pokemon, int* bytes, TipoCola colaMensaje){

	void* stream =  malloc(*bytes);
	int offset = 0;

	memcpy(stream + offset,&colaMensaje, sizeof(TipoCola));
	offset += sizeof(TipoCola);

	memcpy(stream + offset, &pokemon->posX, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->posY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->largoNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon->nombre, pokemon->largoNombre + 1);

	return stream;
}

void* serializarCaught(CaughtPokemon* pokemon, int* bytes, TipoCola colaMensaje){

	void* stream =  malloc(*bytes);
	int offset = 0;

	memcpy(stream + offset,&colaMensaje, sizeof(TipoCola));
	offset += sizeof(TipoCola);

	memcpy(stream + offset, &pokemon->loAtrapo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}

void* serializarGet(GetPokemon* pokemon, int* bytes, TipoCola colaMensaje){

	void* stream =  malloc(*bytes);
	int offset = 0;

	memcpy(stream + offset,&colaMensaje, sizeof(TipoCola));
	offset += sizeof(TipoCola);

	memcpy(stream + offset, &pokemon->largoNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon->nombre, pokemon->largoNombre + 1);

	return stream;
}
