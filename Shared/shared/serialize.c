#include "../shared/serialize.h"
#include "../shared/messages.h"

void* serializarPaquete(Paquete* paquete, int tamanioContenido, int* bytes)
{
	*bytes = sizeof(OpCode) + tamanioContenido;
	void* paqueteSerializado = malloc(*bytes);
	int offset = 0;

	memcpy(paqueteSerializado + offset, &(paquete->codigoOperacion), sizeof(OpCode));
	offset += sizeof(OpCode);

	memcpy(paqueteSerializado + offset, paquete->contenido, tamanioContenido);
	offset += tamanioContenido;

	return paqueteSerializado;
}

Paquete* armarPaquete(OpCode codigoOperacion, void* contenido)
{
	Paquete* paquete = (Paquete*)malloc(sizeof(Paquete));

	paquete->codigoOperacion = codigoOperacion;
	paquete->contenido = contenido;

	return paquete;
}

void* armarPaqueteYSerializar(OpCode codigoOperacion, int tamanioContenido, void* contenido, int* bytes){
	Paquete* paquete = armarPaquete(codigoOperacion, contenido);
	void* paqueteSerializado = serializarPaquete(paquete, tamanioContenido, bytes);

	free(paquete);

	return paqueteSerializado;
}

void* serializarContenidoHaciaBroker(ContenidoHaciaBroker contenido, int tamanioContenido, int* bytes)
{
	*bytes = sizeof(TipoModulo) + tamanioContenido;
	void* contenidoSerializado = malloc(*bytes);
	int offset = 0;

	memcpy(contenidoSerializado + offset, &(contenido.modulo), sizeof(TipoModulo));
	offset += sizeof(TipoModulo);

	memcpy(contenidoSerializado + offset, contenido.contenido, tamanioContenido);
	offset += tamanioContenido;

	return contenidoSerializado;
}

ContenidoHaciaBroker armarContenidoHaciaBroker(TipoModulo modulo, void* contenido)
{
	ContenidoHaciaBroker contenidoFinal;

	contenidoFinal.modulo = modulo;
	contenidoFinal.contenido = contenido;

	return contenidoFinal;
}

void* armarYSerializarContenidoHaciaBroker(TipoModulo modulo, int tamanioContenido, void* contenido, int* bytes)
{
	void* serializacion;
	ContenidoHaciaBroker contenidoFinal = armarContenidoHaciaBroker(modulo, contenido);
	serializacion = serializarContenidoHaciaBroker(contenidoFinal, tamanioContenido, bytes);

	return serializacion;
}

void* serializarAck(Ack contenido, int* bytes)
{
	*bytes = sizeof(ACK);
	void* contenidoSerializado = malloc(*bytes);
	int offset = 0;

	memcpy(contenidoSerializado + offset, &(contenido.IDMensaje), sizeof(long));
	offset += sizeof(long);

	return contenidoSerializado;
}

Ack armarAck(long IDMensaje)
{
	Ack contenido;

	contenido.IDMensaje = IDMensaje;

	return contenido;
}

void* armarYSerializarAck(long IDMensaje, int* bytes)
{
	Ack contenido = armarAck(IDMensaje);
	void* ackSerializado = serializarAck(contenido, bytes);

	return ackSerializado;
}

void* serializarSuscripcion(int cantidadColas, TipoCola* colas, int* tamanio)
{
	*tamanio = sizeof(int) + cantidadColas * sizeof(TipoCola);
	void* stream = malloc(*tamanio);
	int offset = 0;

	memcpy(stream, &cantidadColas, sizeof(int));
	offset += sizeof(int);

	for(int i = 0; i < cantidadColas; i++){
		memcpy(stream + offset, &(colas[i]), sizeof(TipoCola));
		offset += sizeof(TipoCola);
	}

	return stream;
}

Suscripcion* armarSuscripcion(int cantidadColasASuscribir, t_list* colas)
{
	Suscripcion* suscripcion = (Suscripcion*)malloc(sizeof(Suscripcion));

	suscripcion->cantidadColas = cantidadColasASuscribir;
	suscripcion->colas = malloc(sizeof(TipoCola) * cantidadColasASuscribir);

	for(int i = 0; i < cantidadColasASuscribir; i++){
		TipoCola* cola = list_get(colas, i);
		(suscripcion->colas)[i] = *cola;
	}

	return suscripcion;
}

Publicacion armarPublicacion(TipoCola cola, int bytesDato, long IDCorrelativo, void* dato)
{
	Publicacion publicacion;

	publicacion.IDCorrelativo = IDCorrelativo;
	publicacion.cola = cola;
	publicacion.tamanioDato = bytesDato;
	publicacion.dato = dato;

	return publicacion;
}

void* serializarPublicacion(Publicacion publicacion, int* bytes)
{
	int offset = 0;
	*bytes = sizeof(long) + sizeof(TipoCola) + sizeof(int) + publicacion.tamanioDato;
	void* serializacion = malloc(*bytes);

	memcpy(serializacion + offset, &(publicacion.IDCorrelativo), sizeof(long));
	offset += sizeof(long);

	memcpy(serializacion + offset, &(publicacion.cola), sizeof(TipoCola));
	offset += sizeof(TipoCola);

	memcpy(serializacion + offset, &(publicacion.tamanioDato), sizeof(int));
	offset += sizeof(int);

	memcpy(serializacion + offset, publicacion.dato, publicacion.tamanioDato);
	offset += publicacion.tamanioDato;

	return serializacion;
}

void* armarYSerializarPublicacion(TipoCola cola, long IDCorrelativo, void* dato, int bytesDato, int* bytes)
{
	Publicacion publicacion = armarPublicacion(cola, bytesDato, IDCorrelativo, dato);
	void* serializacion = serializarPublicacion(publicacion, bytes);

	return serializacion;
}

void* serializarPublisher(TipoModulo modulo, TipoCola cola, void* dato, long IDCorrelativo, int* bytes)
{
	int bytesDato, bytesPublicacion, bytesContenido;
	void *datoSerializado, *publicacionSerializada, *contenidoSerializado, *serializacionFinal;

	datoSerializado = serializarDato(dato, &bytesDato, cola);
	publicacionSerializada = armarYSerializarPublicacion(cola, IDCorrelativo, datoSerializado, bytesDato, &bytesPublicacion);
	contenidoSerializado = armarYSerializarContenidoHaciaBroker(modulo, bytesPublicacion, publicacionSerializada, &bytesContenido);
	serializacionFinal = armarPaqueteYSerializar(PUBLISHER, bytesContenido, contenidoSerializado, bytes);

	free(datoSerializado);
	free(publicacionSerializada);
	free(contenidoSerializado);

	return serializacionFinal;
}

void* serializarDato(void* mensaje, int* tamanio, TipoCola cola)
{
	switch(cola){
		case NEW:
			return serializarNew(mensaje, tamanio);
			break;
		case APPEARED:
			return serializarAppeared(mensaje, tamanio);
			break;
		case CATCH:
			return serializarCatch(mensaje, tamanio);
			break;
		case CAUGHT:
			return serializarCaught(mensaje, tamanio);
			break;
		case GET:
			return serializarGet(mensaje, tamanio);
			break;
		case LOCALIZED:
			return serializarLocalized(mensaje, tamanio);
			break;
		default:
			return NULL;
			break;
	}
}

void* serializarNew(NewPokemon* pokemon, int* bytes){
	*bytes = sizeof(uint32_t) * 4 + pokemon->largoNombre;
	void* stream =  malloc(*bytes);
	int offset = 0;

	memcpy(stream + offset, &pokemon->posX, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->posY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->cantidad, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->largoNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon->nombre, pokemon->largoNombre);
	offset += pokemon->largoNombre;

	return stream;
}

void* serializarAppeared(AppearedPokemon* pokemon, int* bytes){
	*bytes = sizeof(uint32_t) * 3 + pokemon->largoNombre;
	void* stream =  malloc(*bytes);
	int offset = 0;

	memcpy(stream + offset, &pokemon->posX, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->posY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->largoNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon->nombre, pokemon->largoNombre);
	offset += pokemon->largoNombre;

	return stream;
}

void* serializarCatch(CatchPokemon* pokemon, int* bytes){
	*bytes = sizeof(uint32_t) * 3 + pokemon->largoNombre;
	void* stream =  malloc(*bytes);
	int offset = 0;

	memcpy(stream + offset, &pokemon->posX, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->posY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pokemon->largoNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon->nombre, pokemon->largoNombre);
	offset += pokemon->largoNombre;

	return stream;
}

void* serializarCaught(CaughtPokemon* pokemon, int* bytes){
	*bytes = sizeof(uint32_t);
	void* stream =  malloc(*bytes);
	int offset = 0;

	memcpy(stream + offset, &pokemon->loAtrapo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}

void* serializarGet(GetPokemon* pokemon, int* bytes){
	*bytes = sizeof(uint32_t) + pokemon->largoNombre;
	void* stream =  malloc(*bytes);
	int offset = 0;

	memcpy(stream + offset, &pokemon->largoNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon->nombre, pokemon->largoNombre);
	offset += pokemon->largoNombre;

	return stream;
}

void* serializarLocalized(LocalizedPokemon* pokemon, int* bytes){
	int cantidadPosiciones = list_size(pokemon->posiciones);
	*bytes = sizeof(uint32_t) * 2 + pokemon->largoNombre + cantidadPosiciones * sizeof(uint32_t);
	void* stream = malloc(*bytes);
	int offset = 0;

	memcpy(stream + offset, &pokemon->largoNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, pokemon->nombre, pokemon->largoNombre);
	offset += pokemon->largoNombre;

	memcpy(stream + offset, &pokemon->cantidadDeParesDePosiciones, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for(int i = 0; i < cantidadPosiciones; i++)
	{
		uint32_t* posicion = (uint32_t*)list_get(pokemon->posiciones, i);
		memcpy(stream + offset, posicion, sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

	return stream;
}

NewPokemon* deserializarNew(void* mensaje){
	NewPokemon* pokemon = malloc(sizeof(NewPokemon));
	int offset = 0;

	memcpy(&pokemon->posX, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&pokemon->posY, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pokemon->cantidad, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pokemon->largoNombre, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	char* stringRecibido = malloc(pokemon->largoNombre+1);
	memcpy(stringRecibido, mensaje + offset, pokemon->largoNombre+1);

	pokemon->nombre = malloc(pokemon->largoNombre);
	strncpy(pokemon->nombre, stringRecibido, pokemon->largoNombre);
	pokemon->nombre[pokemon->largoNombre] = '\0';

	return pokemon;
}

AppearedPokemon* deserializarAppeared(void* mensaje){
	AppearedPokemon* pokemon = malloc(sizeof(AppearedPokemon));
	int offset = 0;

	memcpy(&pokemon->posX, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pokemon->posY, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pokemon->largoNombre, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	char* stringRecibido = malloc(pokemon->largoNombre+1);
	memcpy(stringRecibido, mensaje + offset, pokemon->largoNombre + 1);
	offset += pokemon->largoNombre + 1;

	pokemon->nombre = malloc(pokemon->largoNombre);
	strncpy(pokemon->nombre, stringRecibido, pokemon->largoNombre);
	pokemon->nombre[pokemon->largoNombre] = '\0';

	return pokemon;
}

CaughtPokemon* deserializarCaught(void* mensaje){
	CaughtPokemon* pokemon = malloc(sizeof(CaughtPokemon));
	int offset = 0;

	memcpy(&(pokemon->loAtrapo), mensaje + offset, sizeof(uint32_t));

	return pokemon;
}

CatchPokemon* deserializarCatch(void* mensaje){
	CatchPokemon* pokemon = malloc(sizeof(CatchPokemon));
	int offset = 0;

	memcpy(&pokemon->posX, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&pokemon->posY, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&pokemon->largoNombre, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	char* stringRecibido = malloc(pokemon->largoNombre+1);
	memcpy(stringRecibido, mensaje + offset, pokemon->largoNombre+1);

	pokemon->nombre = malloc(pokemon->largoNombre);
	strncpy(pokemon->nombre, stringRecibido, pokemon->largoNombre);
	pokemon->nombre[pokemon->largoNombre] = '\0';

	free(stringRecibido);

	return pokemon;
}

GetPokemon* deserializarGet(void* mensaje){
	GetPokemon* pokemon = malloc(sizeof(GetPokemon));
	int offset = 0;

	memcpy(&pokemon->largoNombre, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	char* stringRecibido = malloc(pokemon->largoNombre+1);

	memcpy(stringRecibido, mensaje + offset, pokemon->largoNombre+1);

	pokemon->nombre = malloc(pokemon->largoNombre);
	strncpy(pokemon->nombre, stringRecibido, pokemon->largoNombre);
	pokemon->nombre[pokemon->largoNombre] = '\0';

	free(stringRecibido);

	return pokemon;
}

LocalizedPokemon* deserializarLocalized(void* mensaje){
	LocalizedPokemon* pokemon = malloc(sizeof(LocalizedPokemon));
	int offset = 0;

	memcpy(&pokemon->largoNombre, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	char* stringRecibido = malloc(pokemon->largoNombre);

	memcpy(stringRecibido, mensaje + offset, pokemon->largoNombre);
	offset += pokemon->largoNombre;
	pokemon->nombre = malloc(pokemon->largoNombre + 1);
	strncpy(pokemon->nombre, stringRecibido, pokemon->largoNombre);
	pokemon->nombre[pokemon->largoNombre] = '\0';
	printf("String pokemon: %s", pokemon->nombre);

	free(stringRecibido);

	memcpy(&pokemon->cantidadDeParesDePosiciones, mensaje + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	printf("Cantidad: %d", pokemon->cantidadDeParesDePosiciones);

	fflush(stdout);

	pokemon->posiciones = list_create();

	for(int i = 0; i < pokemon->cantidadDeParesDePosiciones * 2; i++)
	{
		uint32_t* nuevaPosicion = malloc(sizeof(uint32_t));
		memcpy(nuevaPosicion, mensaje + offset, sizeof(uint32_t));
		list_add(pokemon->posiciones, nuevaPosicion);
		offset += sizeof(uint32_t);
	}

	return pokemon;
}

void* deserializarDato(void* mensaje, TipoCola cola){
	switch(cola){
		case NEW:
			return deserializarNew(mensaje);
			break;
		case APPEARED:
			return deserializarAppeared(mensaje);
			break;
		case CATCH:
			return deserializarCatch(mensaje);
			break;
		case CAUGHT:
			return deserializarCaught(mensaje);
			break;
		case GET:
			return deserializarGet(mensaje);
			break;
		case LOCALIZED:
			return deserializarLocalized(mensaje);
			break;
		default:
			return NULL;
			break;
	}
}

void* serializarStreamIdMensajePublisher(long* ID, TipoCola* cola, int* bytes){
	int offset = 0;
	*bytes = sizeof(long) + sizeof(TipoCola);
	void* stream = malloc(*bytes);

	memcpy(stream + offset, ID, sizeof(long));
	offset += sizeof(long);
	memcpy(stream + offset, cola, sizeof(TipoCola));
	offset += sizeof(TipoCola);

	return stream;
}

void* serializarMensajeSuscriptor(long ID, long IDCorrelativo, void* contenido, int tamanioContenido, TipoCola tipoCola, int* bytes){
	MensajeParaSuscriptor mensajeParaSuscriptor;

	mensajeParaSuscriptor.ID = ID;
	mensajeParaSuscriptor.IDMensajeCorrelativo = IDCorrelativo;
	mensajeParaSuscriptor.cola = tipoCola;
	mensajeParaSuscriptor.tamanioContenido = tamanioContenido;
	mensajeParaSuscriptor.contenido = contenido;

	*bytes = sizeof(long)*2 + sizeof(TipoCola) + sizeof(int) + tamanioContenido;
	void* stream =  malloc(*bytes);
	int offset = 0;

	memcpy(stream + offset,&(mensajeParaSuscriptor.ID), sizeof(long));
	offset += sizeof(long);

	memcpy(stream + offset,&(mensajeParaSuscriptor.IDMensajeCorrelativo), sizeof(long));
	offset += sizeof(long);

	memcpy(stream + offset,&(mensajeParaSuscriptor.cola), sizeof(TipoCola));
	offset += sizeof(TipoCola);

	memcpy(stream + offset, &mensajeParaSuscriptor.tamanioContenido, sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, mensajeParaSuscriptor.contenido, mensajeParaSuscriptor.tamanioContenido);
	offset += mensajeParaSuscriptor.tamanioContenido;

	return stream;
}
