#include "conexionBroker.h"

void manejarSuscripcion(void* contenido, int socket_cliente, t_log* logger){
	int offset = 0;
	int tamanio, numeroActual;
	memcpy(&tamanio, contenido, sizeof(int));
	offset += sizeof(int);

	for(int i = 0; i < tamanio; i++){
		memcpy(&numeroActual, contenido + offset, sizeof(int));
		TipoCola colaASuscribirse = (TipoCola)numeroActual;
		agregarSuscriptor(colaASuscribirse, socket_cliente);
		log_trace(logger, "Nueva cola agregada: %s",tipoColaToString(colaASuscribirse));
		offset += sizeof(int);
	}
}

void manejarPublisher(void* contenido, t_log* logger){
	TipoCola colaAMandar;
	Mensaje nuevoMensaje;
	nuevoMensaje.suscriptoresRecibidos = list_create();
	nuevoMensaje.suscriptoresEnviados = list_create();

	memcpy(&colaAMandar, contenido, sizeof(TipoCola));

	t_list* suscriptores = obtenerSuscriptoresPorCola(colaAMandar);

	if(suscriptores != NULL){
		for(int i = 0; i < list_size(suscriptores); i++){
			int suscriptor = (int)list_get(suscriptores, i);

			if(send(suscriptor, contenido, sizeof(contenido), 0) != -1){
				list_add(nuevoMensaje.suscriptoresRecibidos, &suscriptor);
				//Modificar: debería agregarse a la lista cuando el suscriptor devuelve el ACK.
			}

			list_add(nuevoMensaje.suscriptoresEnviados, &suscriptor);
		}
	}

	agregarMensaje(colaAMandar, nuevoMensaje);
}

void process_request(int cod_op, int cliente_fd){
	t_log* logger = log_create("respuesta.log", "RESPUESTA", true, LOG_LEVEL_TRACE);
	log_trace(logger, "Llegó algo");
	int size;
	void* msg;

	switch (cod_op) {
		case SUSCRIBER:
			log_trace(logger, "Llegó un Suscriber");
			msg = recibirMensajeServidor(cliente_fd, &size);
			log_trace(logger, "Payload: %d", size);
			manejarSuscripcion(msg, cliente_fd, logger);

			break;
		case PUBLISHER:
			log_trace(logger, "Llegó un Publisher");
			msg = recibirMensajeServidor(cliente_fd, &size);
			log_trace(logger, "Payload: %d", size);
			manejarPublisher(msg, logger);

			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
	}
}

void serve_client(int* socket)
{
	int cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	process_request(cod_op, *socket);
}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion;
	int socket_cliente;

	tam_direccion = sizeof(struct sockaddr_in);
	socket_cliente = accept(socket_servidor, (void*)&dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);
}

void iniciar_servidor(char *ip, char* puerto)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor);

}
