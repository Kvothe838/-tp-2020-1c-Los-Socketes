#include "../shared/messages.h"
#include "../shared/serialize.h"

int enviar_mensaje(char* mensaje, int socket_cliente)
{
	int tamanioMensaje = strlen(mensaje) + 1,
		bytes = 0,
		resultado = 0;
	void* mensaje_serializado;
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = NEW;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->stream = mensaje;
	paquete->buffer->size = tamanioMensaje;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, tamanioMensaje);
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

char* recibir_mensaje(int socket_cliente)
{
	op_code codigo_operacion;
	char* stream = NULL;
	int size;

	if(recv(socket_cliente, &codigo_operacion, sizeof(int), 0) <= 0){
		//Si hay error leyendo codigo_operacion o si la conexión se cayó.
		printf("Error recibiendo mensaje.\n");
		return stream;
	}

	switch(codigo_operacion) {
	    case NEW:
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
