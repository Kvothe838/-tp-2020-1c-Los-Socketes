#include "conexionGameCard.h"

void manejarNew(void* contenido, t_log* logger){

}

void serveClient(int* socket)
{
	int size;
	void* msg;
	TipoCola cola;
	t_log* logger;

	logger = log_create("respuesta.log", "RESPUESTA", true, LOG_LEVEL_TRACE);
	log_trace(logger, "Llegó algo\n");
	msg = recibirMensajeServidor(*socket, &size);
	memcpy(msg, &cola, sizeof(TipoCola));
	printf("COLA: %d\n", cola);
	switch (cola) {
		case NEW:
			log_trace(logger, "LLEGÓ EL NEW CARAJO");
			msg = recibirMensajeServidor(*socket, &size);
			log_trace(logger, "Payload: %d", size);
			manejarNew(msg, logger);

			break;
		default:
			log_trace(logger, "Llegó mal :(");
			pthread_exit(NULL);
			break;
	}
}

void esperarCliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion;
	int socket_cliente;

	tam_direccion = sizeof(struct sockaddr_in);
	socket_cliente = accept(socket_servidor, (void*)&dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serveClient,&socket_cliente);
	pthread_detach(thread);
}
