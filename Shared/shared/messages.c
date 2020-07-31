#include "../shared/messages.h"
#include "../shared/serialize.h"
#include "../shared/structs.h"

int enviarPublisherSinIDCorrelativo(t_log* logger, int socket, TipoModulo modulo, void* dato, TipoCola cola,
		IDMensajePublisher** mensajeRecibido)
{
	int IDCorrelativoVacio = 0;

	return enviarPublisherConIDCorrelativo(logger, socket, modulo, dato, cola, IDCorrelativoVacio, mensajeRecibido);
}

int enviarPublisherConIDCorrelativo(t_log* logger, int socket, TipoModulo modulo, void* dato, TipoCola cola, long IDCorrelativo,
		IDMensajePublisher** mensajeRecibido)
{
	int resultado = 1;
	int bytes;
	void* paquete = serializarPublisher(modulo, cola, dato, IDCorrelativo, &bytes);

	if(send(socket, paquete, bytes, 0) == -1)
	{
		log_info(logger, "Mensaje enviado.");
		resultado = 0;
	}

	free(paquete);

	if(resultado == 0) return 0;

	OpCode code;

	if(recv(socket, &code, sizeof(OpCode), 0) == -1)
	{
		return 0;
	}

	if(code == ID_MENSAJE){
		*mensajeRecibido = malloc(sizeof(IDMensajePublisher));
		recibirIDMensajePublisher(socket, *mensajeRecibido);

	} else {
		log_info(logger, "Error al recibir IDMensaje para Publisher.");
	}

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
		int hiloTeam = modulo - TEAM;
		char str[6];
		char* stringCompleto;

		sprintf(str, "TEAM%d", hiloTeam);

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

LocalizedPokemon* getLocalized(char* nombre, int cantidadPosiciones, ...){
	va_list posiciones;

	va_start(posiciones, cantidadPosiciones); //Esto tira un warning pelotudísimo, IGNORAR.
	t_list* listaDePosiciones = list_create();

	for(int i = 0; i < cantidadPosiciones; i++){
		uint32_t* nuevaPosicion = malloc(sizeof(uint32_t));
		*nuevaPosicion = va_arg(posiciones, uint32_t);
		list_add(listaDePosiciones, nuevaPosicion);
	}

	va_end(posiciones);

	return getLocalizedConList(nombre, cantidadPosiciones / 2, listaDePosiciones);
}

LocalizedPokemon* getLocalizedConList(char* nombre, int cantidadParesPosiciones, t_list* posiciones){
	LocalizedPokemon* pokemon = malloc(sizeof(LocalizedPokemon));

	pokemon->nombre = nombre;
	pokemon->largoNombre = strlen(pokemon->nombre);
	pokemon->cantidadDeParesDePosiciones = cantidadParesPosiciones;
	pokemon->posiciones = posiciones;

	return pokemon;
}

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

	respuesta = armarYSerializarAck((*mensaje)->ID, &bytes);

	send(socket, respuesta, bytes, 0);

	log_info(logger, "ACK enviado para mensaje con ID %d", (*mensaje)->ID);

	return 1; //Retornar 0 en caso de error en algún recv.
}

int recibirIDMensajePublisher(int socket, IDMensajePublisher* mensaje)
{
	recv(socket, &(mensaje->IDMensaje), sizeof(long), MSG_WAITALL);
	recv(socket, &(mensaje->cola), sizeof(TipoCola), MSG_WAITALL);

	return 1;
}

