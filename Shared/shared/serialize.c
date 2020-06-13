#include "../shared/serialize.h"
//serializar_get
void* serializarPaquete(Paquete* paquete, int *bytes)
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

Paquete* armarPaquete(OpCode codigoOperacion, int tamanio, void* stream){
	Paquete* paquete = malloc(sizeof(Paquete));

	paquete->codigoOperacion = codigoOperacion;
	paquete->buffer = malloc(sizeof(Buffer));
	paquete->buffer->size = tamanio;
	paquete->buffer->stream = stream;

	return paquete;
}

void* armarPaqueteYSerializar(OpCode codigoOperacion, int tamanio, void* stream, int* bytes){
	Paquete* paquete = armarPaquete(codigoOperacion, tamanio, stream);
	void* paqueteSerializado = serializarPaquete(paquete, bytes);

	free(paquete->buffer);
	free(paquete);

	return paqueteSerializado;
}

void* serializarSuscripcion(Suscripcion* suscripcion, int tamanio){
	void* stream = malloc(tamanio);
	int offset = 0;

	memcpy(stream, &suscripcion->cantidadColas, sizeof(int));
	offset += sizeof(int);

	for(int i = 0; i < suscripcion->cantidadColas; i++){
		memcpy(stream + offset, &((suscripcion->colas)[i]), sizeof(TipoCola));
		offset += sizeof(TipoCola);
	}

	return stream;
}

void* serializarDato(void* mensaje, int tamanioMensaje, TipoCola colaMensaje){
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
			return NULL;
			break;
	}

}

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
	offset += pokemon->largoNombre + 1;

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
	offset += pokemon->largoNombre + 1;

	return stream;
}

void* serializarCatch(CatchPokemon* pokemon, int* bytes, TipoCola colaMensaje){

	void* stream =  malloc(*bytes + sizeof(TipoCola));
	int offset = 0;

	memcpy(stream + offset, &colaMensaje, sizeof(TipoCola));
	offset += sizeof(TipoCola);

	memcpy(stream + offset, &pokemon->posX, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->posY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->largoNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon->nombre, pokemon->largoNombre + 1);
	offset += pokemon->largoNombre + 1;

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

	void* stream =  malloc(*bytes + sizeof(TipoCola));
	int offset = 0;

	memcpy(stream + offset,&colaMensaje, sizeof(TipoCola));
	offset += sizeof(TipoCola);

	memcpy(stream + offset, &pokemon->largoNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon->nombre, pokemon->largoNombre + 1);
	offset += pokemon->largoNombre + 1;

	return stream;
}

NewPokemon* deserializarNew(void* msj, int* bytes){

	NewPokemon* pokemon = malloc(sizeof(NewPokemon));
	uint32_t pos = 0;
	memcpy(&pokemon->posX, msj, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	memcpy(&pokemon->posY, msj+pos, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	memcpy(&pokemon->cantidad, msj+pos, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	memcpy(&pokemon->largoNombre, msj+pos, sizeof(uint32_t));
	pos += sizeof(uint32_t);

	char* stringRecibido = malloc(pokemon->largoNombre+1);
	memcpy(stringRecibido, msj+pos, pokemon->largoNombre+1);

	pokemon->nombre = malloc(pokemon->largoNombre);
	strncpy(pokemon->nombre, stringRecibido, pokemon->largoNombre);
	pokemon->nombre[pokemon->largoNombre] = '\0';

	return pokemon;
}

AppearedPokemon* deserializarAppeared(void* msj, int* bytes){

	AppearedPokemon* pokemon = malloc(sizeof(AppearedPokemon));
	uint32_t pos = 0;
	memcpy(&pokemon->posX, msj, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	memcpy(&pokemon->posY, msj+pos, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	memcpy(&pokemon->largoNombre, msj+pos, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	memcpy(pokemon->nombre, msj+pos, pokemon->largoNombre + 1);

	return pokemon;
}

CaughtPokemon* deserializarCaught(void* msj, int* bytes){

	CaughtPokemon* pokemon = malloc(sizeof(CaughtPokemon));

	memcpy(&pokemon->loAtrapo, msj, sizeof(uint32_t));

	return pokemon;
}

CatchPokemon* deserializarCatch(void* msj, int* bytes){

	CatchPokemon* pokemon = malloc(sizeof(CatchPokemon));
	uint32_t pos = 0;
	memcpy(&pokemon->posX, msj, sizeof(uint32_t));

	pos += sizeof(uint32_t);
	memcpy(&pokemon->posY, msj+pos, sizeof(uint32_t));

	pos += sizeof(uint32_t);
	memcpy(&pokemon->largoNombre, msj+pos, sizeof(uint32_t));

	pos += sizeof(uint32_t);

	char* stringRecibido = malloc(pokemon->largoNombre+1);
	memcpy(stringRecibido, msj+pos, pokemon->largoNombre+1);

	pokemon->nombre = malloc(pokemon->largoNombre);
	strncpy(pokemon->nombre, stringRecibido, pokemon->largoNombre);
	pokemon->nombre[pokemon->largoNombre] = '\0';

	free(stringRecibido);


	return pokemon;
}

GetPokemon* deserializarGet(void* msj, int* bytes){

	GetPokemon* pokemon = malloc(sizeof(GetPokemon));

	uint32_t pos = 0;
	memcpy(&pokemon->largoNombre, msj, sizeof(uint32_t));
	pos += sizeof(uint32_t);

	char* stringRecibido = malloc(pokemon->largoNombre+1);

	memcpy(stringRecibido, msj+pos, pokemon->largoNombre+1);

	pokemon->nombre = malloc(pokemon->largoNombre);
	strncpy(pokemon->nombre, stringRecibido, pokemon->largoNombre);
	pokemon->nombre[pokemon->largoNombre] = '\0';

	free(stringRecibido);

	return pokemon;
}
void* deserializarDato(void* msj,int* bytes){
	TipoCola tipo;

	memcpy(&tipo, msj , sizeof(TipoCola));

	switch(tipo){
			case NEW:
				return deserializarNew(msj,bytes);
				break;
			case APPEARED:
				return deserializarAppeared(msj,bytes);
				break;
			case CATCH:
				return deserializarCatch(msj,bytes);
				break;
			case CAUGHT:
				return deserializarCaught(msj,bytes);
				break;
			case GET:
				return deserializarGet(msj,bytes);
				break;
			default:
				return NULL;
				break;
		}
}

void* serializarStreamIdMensajePublisher(long ID, TipoCola cola){
	void* stream = malloc(sizeof(long) + sizeof(TipoCola));

	memcpy(stream, &ID, sizeof(long));
	memcpy(stream + sizeof(long), &cola, sizeof(TipoCola));

	return stream;
}

void* serializarMensajeSuscriptor(MensajeEnCola mensajeEnCola, TipoCola tipoCola){
	MensajeParaSuscriptor mensajeParaSuscriptor;

	mensajeParaSuscriptor.ID = mensajeEnCola.ID;
	mensajeParaSuscriptor.IDMensajeCorrelativo = mensajeEnCola.IDCorrelativo;
	mensajeParaSuscriptor.cola = tipoCola;
	mensajeParaSuscriptor.sizeContenido = sizeof(mensajeEnCola.contenido);
	mensajeParaSuscriptor.contenido = mensajeEnCola.contenido;

	void* stream =  malloc(sizeof(MensajeParaSuscriptor));
	int offset = 0;

	memcpy(stream + offset,&(mensajeParaSuscriptor.ID), sizeof(long));
	offset += sizeof(long);

	memcpy(stream + offset,&(mensajeParaSuscriptor.IDMensajeCorrelativo), sizeof(long));
	offset += sizeof(long);

	memcpy(stream + offset,&(mensajeParaSuscriptor.cola), sizeof(TipoCola));
	offset += sizeof(TipoCola);

	memcpy(stream + offset,&(mensajeParaSuscriptor.sizeContenido), sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset,&(mensajeParaSuscriptor.contenido), mensajeParaSuscriptor.sizeContenido);
	offset += mensajeParaSuscriptor.sizeContenido;

	return stream;
}
