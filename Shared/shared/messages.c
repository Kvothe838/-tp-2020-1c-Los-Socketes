#include "../shared/messages.h"
#include "../shared/serialize.h"
#include "../shared/structs.h"

/* Envía un mensaje según opcode.
 * Si opcode es suscriber, el formato va a ser:

   {
   	   cantidad de colas (int)
   	   array de colas (array de TipoCola)
   }

 * Si opcode es publisher, el formato va a ser:

   {
   	   id corelativo (long) (-1 si es null)
   	   modulo (TipoModulo)
   	   tipo de cola (TipoCola)
   	   mensaje puro
   }

   Nota: no hace falta enviar el tamaño del mensaje antes que éste, ya que con saber el tipo de cola, el receptor
   ya sabe cómo deserializarlo.
*/
int enviarMensaje(void* mensaje, OpCode codigoOperacion, TipoCola cola, long* IDCorrelativo, int socket_cliente,
		TipoModulo modulo)
{
	int bytes, offset = 0, resultado = 0, tamanioTotal = sizeof(TipoCola), tamanioMensaje;
	void *mensajeSerializado, *streamMensajeEspecifico, *stream;

	switch(codigoOperacion){
		case SUSCRIBER:
			streamMensajeEspecifico = serializarSuscripcion(mensaje, &tamanioMensaje);
			break;
		case PUBLISHER:
			streamMensajeEspecifico = serializarDato(mensaje, &tamanioMensaje, cola);
			break;
		default:
			break;
	}

	tamanioTotal += tamanioMensaje;

	if(IDCorrelativo == NULL && codigoOperacion == PUBLISHER){
		long IDCorrelativoVacio = -1;
		IDCorrelativo = &IDCorrelativoVacio;
	}

	if(IDCorrelativo != NULL){
		stream = malloc(tamanioMensaje + sizeof(long));
		memcpy(stream + offset, IDCorrelativo, sizeof(long));
		offset += sizeof(long);
		tamanioTotal += sizeof(long);
	} else {
		stream = malloc(tamanioMensaje);
	}

	memcpy(stream + offset, &cola, sizeof(TipoCola));
	offset += sizeof(TipoCola);

	memcpy(stream + offset, streamMensajeEspecifico, tamanioMensaje);
	offset += tamanioMensaje;

	mensajeSerializado = armarPaqueteYSerializar(codigoOperacion, tamanioTotal, modulo, stream, &bytes);

	if(send(socket_cliente, mensajeSerializado, bytes, 0) == -1){
		printf("Error enviando mensaje.\n");
		resultado = -1;
	}

	free(mensajeSerializado);
	return resultado;
}

void mandarSuscripcion(int socket_server, TipoModulo modulo, int cantidadColasASuscribir, ...){
	va_list colas;
	va_start(colas, cantidadColasASuscribir);
	Suscripcion* suscripcion = malloc(sizeof(Suscripcion));
	suscripcion->cantidadColas = cantidadColasASuscribir;
	suscripcion->colas = malloc(sizeof(TipoCola) * cantidadColasASuscribir);

	for(int i = 0; i < cantidadColasASuscribir; i++){
		TipoCola cola = va_arg(colas, TipoCola);
		(suscripcion->colas)[i] = cola;
	}

	va_end(colas);

	int resultado = enviarMensaje(suscripcion, SUSCRIBER, 0, NULL, socket_server, modulo);

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

/* Envía un nuevo mensaje de una cola a un suscriptor. Está acá porque puede ser usado tanto por GameBoy como por Broker.
 * Formato:
     {
     	 id correlativo (long)
     	 tipo de cola (TipoCola)
     	 mensaje (void*)
     }

     Nota: no hace falta enviar el tamaño del mensaje antes que éste, ya que con saber el tipo de cola, el receptor
   	 ya sabe cómo deserializarlo.
*/

int enviarMensajeASuscriptor(int socketSuscriptor, long ID, long* IDCorrelativo, TipoCola cola, void* data){
	int tamanioFinal = sizeof(long) + sizeof(TipoCola);
	int resultado = 1, *tamanioDato = NULL;
	void *mensajeAEnviar, *datoSerializado;

	datoSerializado = serializarDato(data, tamanioDato, cola);
	tamanioFinal += *tamanioDato;
	mensajeAEnviar = serializarMensajeSuscriptor(ID, IDCorrelativo, datoSerializado, *tamanioDato, cola);

	if(send(socketSuscriptor, mensajeAEnviar, tamanioFinal, 0) == -1){
		resultado = 0;
	}

	free(mensajeAEnviar);

	return resultado;
}

