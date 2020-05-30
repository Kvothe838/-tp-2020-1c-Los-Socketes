#include "../shared/messages.h"
#include "../shared/serialize.h"
#include "../shared/structs.h"

int enviarMensaje(void* mensaje, int tamanioMensaje, OpCode codMensaje, TipoCola colaMensaje, int socket_cliente)
{
	int bytes = 0,
		resultado = 0;
	void* mensaje_serializado;

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
			stream = serializarDato(mensaje, tamanioMensaje, stream, colaMensaje);
			break;
		default:
			break;
	}

	paquete->buffer->stream = stream;

	mensaje_serializado = serializar_paquete(paquete, &bytes);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	if(send(socket_cliente, mensaje_serializado, bytes, 0) == -1){
		printf("Error enviando mensaje.\n");
		resultado = -1;
	}

	free(mensaje_serializado);
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
