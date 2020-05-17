#include <shared/connection.h>
#include "conexionBroker.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>

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

void process_request(int cod_op, int cliente_fd){
	t_log* logger = log_create("respuesta.log", "RESPUESTA", true, LOG_LEVEL_TRACE);
	log_trace(logger, "Llegó algo");
	int size;
	void* msg;

	switch (cod_op) {
		case SUSCRIBER:
			log_trace(logger, "Llegó un SUSCRIBER");
			msg = recibirMensajeServidor(cliente_fd, &size);
			log_trace(logger, "Payload: %d", size);
			manejarSuscripcion(msg, cliente_fd, logger);

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
