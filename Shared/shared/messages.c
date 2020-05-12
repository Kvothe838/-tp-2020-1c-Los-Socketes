#include "../shared/messages.h"
#include "../shared/serialize.h"


void* serializarSuscripcion(Suscripcion* suscripto, int tamanio, void* stream){
	stream = malloc(tamanio);
	void *prueba = malloc(tamanio);
	int ayuda, offset = 0, offset2 = 0, imprimir;
	memcpy(stream, &suscripto->cantidadDeColas, sizeof(int));

	memcpy(&ayuda, stream, sizeof(int));

	printf("CANTIDAD DE COLAS: %d\n", ayuda);

	offset += sizeof(tipoCola);
	memcpy(&ayuda, stream, sizeof(int));

	prueba = malloc(sizeof(int) * ayuda);
	memcpy(prueba, (suscripto->colas), sizeof(int) * ayuda);

	for(int i = 0; i < ayuda; i++){

		memcpy(stream + offset, (prueba + offset - sizeof(int)), sizeof(int));
		memcpy(&imprimir, stream + offset, sizeof(int));
		printf("DATO %d IMPRIMIR: %d\n", i, imprimir);
		offset += sizeof(int);

	}
	return stream;
}


int enviar_mensaje(void* mensaje, int tamanioMensaje, opCode codMensaje, int socket_cliente)
{
	int bytes = 0,
		resultado = 0;
	void* mensaje_serializado;

	t_paquete* paquete = NULL;
	paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = codMensaje;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = tamanioMensaje;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	void * stream = malloc(paquete->buffer->size);

	switch(codMensaje){
		case SUSCRIBER:
			stream = serializarSuscripcion(mensaje, tamanioMensaje, stream);
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
		printf("Error enviando mensaje.");
		resultado = -1;
	}

	free(mensaje_serializado);
	return resultado;
}

void mandarSuscripcion(int cantidadDeColasASuscribir, tipoCola colas[], int socket_server){
	Suscripcion* suscripcion = malloc(sizeof(Suscripcion));
	int offset = 0, tamanioFinal;

	suscripcion->cantidadDeColas = cantidadDeColasASuscribir;

	suscripcion->colas = malloc(sizeof(tipoCola) * suscripcion->cantidadDeColas);
	void *auxiliar = malloc(suscripcion->colas);

	for(int i = 0; i < cantidadDeColasASuscribir; i++){

		memcpy(auxiliar + offset, &(colas[i]), sizeof(int));
		offset += sizeof(int);

	}

	suscripcion->colas = auxiliar;

	tamanioFinal = cantidadDeColasASuscribir*sizeof(int) + sizeof(int);

	int resultado = enviar_mensaje(suscripcion, tamanioFinal, SUSCRIBER, socket_server);
	if(resultado == -1)
		printf("ERROR");
}
void* recibir_mensaje(int socket_cliente)
{
	tipoCola codigo_operacion;
	void* stream = NULL;
	int size;

	if(recv(socket_cliente, &codigo_operacion, sizeof(int), 0) <= 0){
		//Si hay error leyendo codigo_operacion o si la conexión se cayó.
		printf("Error recibiendo mensaje.\n");
		return stream;
	}

	switch(codigo_operacion) {
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

void* recibir_mensaje_servidor(int socket_cliente, int* size)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}
