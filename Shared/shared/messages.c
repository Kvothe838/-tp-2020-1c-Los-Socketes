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

/*int enviarMensaje(void* mensaje, OpCode codigoOperacion, TipoCola cola, long* IDCorrelativo, int socket_cliente,
		TipoModulo modulo)
{
	int bytes, offset = 0, resultado = 0, tamanioTotal = 0, tamanioMensaje;
	void *mensajeSerializado, *streamMensajeEspecifico, *stream;

	switch(codigoOperacion){
		case SUSCRIBER:
			streamMensajeEspecifico = serializarSuscripcion(modulo, &bytes);
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
		tamanioTotal += sizeof(long);
		stream = malloc(tamanioTotal);
		memcpy(stream + offset, IDCorrelativo, sizeof(long));
		offset += sizeof(long);
	} else {
		stream = malloc(tamanioTotal);
	}

	if(codigoOperacion != SUSCRIBER){
		memcpy(stream + offset, &cola, sizeof(TipoCola));
		offset += sizeof(TipoCola);
		tamanioTotal += sizeof(TipoCola);
	}

	memcpy(stream + offset, streamMensajeEspecifico, tamanioMensaje);
	offset += tamanioMensaje;

	mensajeSerializado = armarPaqueteYSerializar(codigoOperacion, modulo, tamanioTotal, stream, &bytes);

	if(send(socket_cliente, mensajeSerializado, bytes, 0) == -1){
		printf("Error enviando mensaje.\n");
		resultado = -1;
	}

	free(mensajeSerializado);
	return resultado;
}*/

int enviarPublisherSinIDCorrelativo(int socket, TipoModulo modulo, void* dato, TipoCola cola)
{
	int IDCorrelativoVacio = 0;

	return enviarPublisherConIDCorrelativo(socket, modulo, dato, cola, IDCorrelativoVacio);
}

int enviarPublisherConIDCorrelativo(int socket, TipoModulo modulo, void* dato, TipoCola cola, long IDCorrelativo)
{
	int resultado = 1;
	int bytes;
	void* paquete = serializarPublisher(modulo, cola, dato, IDCorrelativo, &bytes);

	if(send(socket, paquete, bytes, 0) == -1)
	{
		resultado = 0;
	}

	free(paquete);

	return resultado;
}

int enviarSuscripcion(int socket, TipoModulo modulo, int cantidadColas, ...){
	int resultado = 1;
	int bytes, bytesContenido, bytesSuscripcion;
	void *suscripcionSerializada, *contenidoSerializado, *serializacionFinal;
	va_list colasRecibidas;

	va_start(colasRecibidas, cantidadColas);
	TipoCola* colas = malloc(sizeof(TipoCola)*cantidadColas);

	for(int i = 0; i < cantidadColas; i++){
		TipoCola cola = va_arg(colasRecibidas, TipoCola);
		colas[i] = cola;
	}

	va_end(colasRecibidas);

	suscripcionSerializada = serializarSuscripcion(cantidadColas, colas, &bytesSuscripcion);
	contenidoSerializado = armarYSerializarContenidoHaciaBroker(modulo, bytesSuscripcion, suscripcionSerializada, &bytesContenido);
	serializacionFinal = armarPaqueteYSerializar(SUSCRIBER, bytesContenido, contenidoSerializado, &bytes);

	if(send(socket, serializacionFinal, bytes, 0) == -1)
	{
		resultado = -1;
	}

	free(serializacionFinal);

	return resultado;
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

char* tipoModuloToString(TipoModulo modulo){
	if(modulo > TEAM){
		char str[6];
		char* stringCompleto;

		sprintf(str, "TEAM%d", modulo);

		stringCompleto = malloc(6 * sizeof(char));

		for(int i = 0; i < 5; i++){
			stringCompleto[i] = str[i];
		}

		stringCompleto[5] = '\0';

		return stringCompleto;
	}

	switch(modulo){
		case BROKER:
			return "BROKER";
		case GAMECARD:
			return "GAMECARD";
		case GAMEBOY:
			return "GAMEBOY";
		case TEAM:
			return "TEAM";
	}

	return "";
}

ModuloGameboy argvToModuloGameboy(char* modulo)
{
	if(strcmp(modulo,"BROKER") == 0)
		return BROKER_GAMEBOY;
	else if(strcmp(modulo,"TEAM") == 0)
		return TEAM_GAMEBOY;
	else if(strcmp(modulo,"GAMECARD") == 0)
		return GAMECARD_GAMEBOY;
	else if(strcmp(modulo,"SUSCRIPTOR") == 0)
			return SUSCRIPTOR_GAMEBOY;
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

int enviarMensajeASuscriptor(int socketSuscriptor, long ID, long IDCorrelativo, TipoCola cola, void* data){
	int resultado = 1, tamanioDato, bytes;
	void *mensajeAEnviar, *datoSerializado;

	datoSerializado = serializarDato(data, &tamanioDato, cola);
	mensajeAEnviar = serializarMensajeSuscriptor(ID, IDCorrelativo, datoSerializado, tamanioDato, cola, &bytes);

	if(send(socketSuscriptor, mensajeAEnviar, bytes, 0) == -1){
		resultado = 0;
	}

	free(mensajeAEnviar);

	return resultado;
}

int recibirMensajeSuscriber(int socket, t_log* logger, TipoModulo modulo, MensajeParaSuscriptor** mensaje, char* ip, char* puerto){
	*mensaje = (MensajeParaSuscriptor*)malloc(sizeof(MensajeParaSuscriptor));
	void *respuesta;
	int bytes;

	recv(socket, &((*mensaje)->ID), sizeof(long), 0);
	recv(socket, &((*mensaje)->IDMensajeCorrelativo), sizeof(long), 0);
	recv(socket, &((*mensaje)->cola), sizeof(TipoCola), 0);
	recv(socket, &((*mensaje)->tamanioContenido), sizeof(int), 0);
	(*mensaje)->contenido = malloc((*mensaje)->tamanioContenido);
	recv(socket, (*mensaje)->contenido, (*mensaje)->tamanioContenido, 0);

	log_info(logger, "Nuevo mensaje recibido con ID %d de cola %s", (*mensaje)->ID, tipoColaToString((*mensaje)->cola));

	respuesta = armarYSerializarAck((*mensaje)->ID, modulo, &bytes);

	int nuevoSocket = crear_conexion_cliente(ip, puerto);

	send(nuevoSocket, respuesta, bytes, 0);

	log_info(logger, "ACK enviado para mensaje con ID %d", (*mensaje)->ID);

	liberar_conexion_cliente(nuevoSocket);

	return 1; //Retornar 0 en caso de error en algún recv.
}

int recibirIDMensajePublisher(int socket, IDMensajePublisher* mensaje)
{
	recv(socket, &(mensaje->IDMensaje), sizeof(long), MSG_WAITALL);
	recv(socket, &(mensaje->cola), sizeof(TipoCola), MSG_WAITALL);

	return 1;
}

