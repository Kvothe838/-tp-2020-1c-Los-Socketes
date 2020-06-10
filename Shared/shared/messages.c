#include "../shared/messages.h"
#include "../shared/serialize.h"
#include "../shared/structs.h"

int enviarMensaje(void* mensaje, int tamanioMensaje, OpCode codMensaje, TipoCola colaMensaje, int socket_cliente)
{
	int bytes = 0,
		resultado = 0;
	void* mensajeSerializado;

	Paquete* paquete = NULL;
	paquete = malloc(sizeof(Paquete));

	paquete->codigoOperacion = codMensaje;
	paquete->buffer = malloc(sizeof(Buffer));
	paquete->buffer->size = tamanioMensaje;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	void * stream = malloc(paquete->buffer->size);

	switch(codMensaje){
		case SUSCRIBER:
			stream = serializarSuscripcion(mensaje, tamanioMensaje, stream);
			break;
		case PUBLISHER:
			stream = serializarDato(mensaje, tamanioMensaje, colaMensaje);
			break;
		default:
			break;
	}

	paquete->buffer->stream = stream;

	mensajeSerializado = serializarPaquete(paquete, &bytes);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	if(send(socket_cliente, mensajeSerializado, bytes, 0) == -1){
		printf("Error enviando mensaje.\n");
		resultado = -1;
	}

	free(mensajeSerializado);
	return resultado;
}

void mandarSuscripcion(int socket_server, int cantidadColasASuscribir, ...){
	va_list colas;
	va_start(colas, cantidadColasASuscribir);
	Suscripcion* suscripcion = malloc(sizeof(Suscripcion));
	int offset = 0, tamanioFinal;
	suscripcion->cantidadColas = cantidadColasASuscribir;
	void *auxiliar = malloc(sizeof(TipoCola) * suscripcion->cantidadColas);

	for(int i = 0; i < cantidadColasASuscribir; i++){
		TipoCola cola = va_arg(colas, TipoCola);
		memcpy(auxiliar + offset, &cola, sizeof(int));
		offset += sizeof(int);
	}

	va_end(colas);
	suscripcion->colas = auxiliar;
	tamanioFinal = cantidadColasASuscribir*sizeof(int) + sizeof(int);
	int resultado = enviarMensaje(suscripcion, tamanioFinal, SUSCRIBER, 0, socket_server);

	if(resultado == -1)
		printf("ERROR");
}

void* recibirMensaje(int socket_cliente)
{
	TipoCola codigoOperacion;
	void* stream = NULL;
	int size;

	if(recv(socket_cliente, &codigoOperacion, sizeof(int), 0) <= 0){
		//Si hay error leyendo codigo_operacion o si la conexión se cayó.
		printf("Error recibiendo mensaje.\n");
		return stream;
	}

	switch(codigoOperacion) {
	    case SUSCRIBER:
	    	recv(socket_cliente, &size, sizeof(int), 0);
			stream = malloc(size);
			recv(socket_cliente, stream, size, 0);
	        break;
	    default:
	    	printf("Error: código no válido.\n");
	}

	return stream;
}

void* recibirMensajeServidor(int socket_cliente, int* size)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

char* tipoColaToString(TipoCola tipoCola){
	switch(tipoCola){
		case NEW:
			return "NEW";
		case GET:
			return "GET";
		case CATCH:
			return "CATCH";
		case APPEARED:
			return "APPEARED";
		case LOCALIZED:
			return "LOCALIZED";
		case CAUGHT:
			return "CAUGHT";
	}

	return "";
}

TipoModulo argvToTipoModulo(char* modulo)
{
	if(strcmp(modulo,"BROKER") == 0)
		return BROKER;
	else if(strcmp(modulo,"TEAM") == 0)
		return TEAM;
	else if(strcmp(modulo,"GAMECARD") == 0)
		return GAMECARD;
	else if(strcmp(modulo,"SUSCRIPTOR") == 0)
		return SUSCRIPTOR;
	else
		return -1;
}

TipoCola argvToTipoCola(char* cola)
{
	if(strcmp(cola,"NEW_POKEMON") == 0)
		return NEW;
	else if(strcmp(cola,"APPEARED_POKEMON") == 0)
		return APPEARED;
	else if(strcmp(cola,"CATCH_POKEMON") == 0)
		return CATCH;
	else if(strcmp(cola,"CAUGHT_POKEMON") == 0)
		return CAUGHT;
	else if(strcmp(cola,"GET_POKEMON") == 0)
		return GET;
	else
		return -1;
}

NewPokemon* getNewPokemon(char* nombre, int posX, int posY, int cantidad){
	NewPokemon* pokemon = malloc(sizeof(NewPokemon));
	pokemon->nombre = nombre;
	pokemon->largoNombre = strlen(pokemon->nombre);
	pokemon->posX = posX;
	pokemon->posY = posY;
	pokemon->cantidad = cantidad;
	return pokemon;
}

AppearedPokemon* getAppearedPokemon(char* nombre, int posX, int posY){
	AppearedPokemon* pokemon = malloc(sizeof(AppearedPokemon));
	pokemon->nombre = nombre;
	pokemon->largoNombre = strlen(pokemon->nombre);
	pokemon->posX = posX;
	pokemon->posY = posY;
	return pokemon;
}

CatchPokemon* getCatchPokemon(char* nombre, int posX, int posY){
	CatchPokemon* pokemon = malloc(sizeof(CatchPokemon));
	pokemon->nombre = nombre;
	pokemon->largoNombre = strlen(pokemon->nombre);
	pokemon->posX = posX;
	pokemon->posY = posY;
	return pokemon;
}

CaughtPokemon* getCaughtPokemon(int loAtrapo){
	CaughtPokemon* pokemon = malloc(sizeof(CaughtPokemon));
	pokemon->loAtrapo = loAtrapo; //si se pudo o no atrapar al pokemon (0 o 1)
	return pokemon;
}

GetPokemon* getGetPokemon(char* nombre){
	GetPokemon* pokemon = malloc(sizeof(GetPokemon));
	pokemon->nombre = nombre;
	pokemon->largoNombre = strlen(pokemon->nombre);
	return pokemon;
}

uint32_t enviarMensajeASuscriptor(uint32_t socketSuscriptor, long IDCorrelativo, TipoCola colaDeSalida, void* data, uint32_t tamanioData){
	uint32_t tamanioFinal = tamanioData + sizeof(long) + sizeof(TipoCola) + sizeof(uint32_t);
	uint32_t resultado = 1;

	void* mensajeAEnviar = malloc(tamanioFinal);
	void* datoSerializado = NULL;
	uint32_t offset = 0;


	memcpy(mensajeAEnviar, &IDCorrelativo, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensajeAEnviar + offset, &tamanioData, sizeof(long));
	offset += sizeof(long);

	datoSerializado = serializarDato(data, tamanioData, colaDeSalida);

	memcpy(mensajeAEnviar + offset, datoSerializado, sizeof(TipoCola) + tamanioData);
	offset += tamanioData;

	if(send(socketSuscriptor, mensajeAEnviar, tamanioFinal, 0) == -1){
		printf("Error enviando mensaje.\n");
		resultado = 0;
	}

	free(mensajeAEnviar);
	//free(datoSerializado);

	return resultado;
}

