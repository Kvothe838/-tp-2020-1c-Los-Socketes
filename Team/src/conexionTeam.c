#include "conexionTeam.h"
#include "team.h"
#include <shared/connection.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>
#include<shared/serialize.h>

void* obtenerData(TipoCola cola,void* msj,int size,t_log* logger){
	AppearedPokemon* dataPokemon;
	switch(cola){
	case APPEARED:
		dataPokemon = deserializarDato(&msj,size);
		log_trace(logger,"Nombre: %s",dataPokemon->nombre);
		log_trace(logger,"Pos X : %d",dataPokemon->posX);

		log_trace(logger,"Pos Y : %d",dataPokemon->posY);
		break;
	default:
		break;
	}
}
void manejarMensaje(void* msj, t_log* logger,int size){
	Pokemon* pokemon = malloc(sizeof(Pokemon));
	int offset = 0;
	int numeroActual;
	int largoNombre;
	TipoCola tipoCola;
	memcpy(&numeroActual, msj + offset, sizeof(int));
	tipoCola = (TipoCola) numeroActual;
	log_trace(logger, "TIPO DE COLA: %s",tipoColaToString(tipoCola));
	offset += sizeof(int);
	memcpy(&(pokemon->x),msj + offset,sizeof(int));
	offset+= sizeof(int);
	log_trace(logger,"X: %d",pokemon->x);
	memcpy(&(pokemon->y),msj + offset,sizeof(int));
	offset+= sizeof(int);
	log_trace(logger,"X: %d",pokemon->y);

	memcpy(&(largoNombre), msj+offset, sizeof(int));
	offset += sizeof(int);

	char* nombre;
	log_trace(logger,"Largo: %d",largoNombre);
	memcpy(nombre, msj+offset , largoNombre +1);
	pokemon->nombre = nombre;
	log_trace(logger, "Nombre : %s",pokemon->nombre);



	free(pokemon);
	//obtenerData(tipoCola,msj,size,logger);
}

void process_request(int cod_op, int cliente_fd){
	t_log* logger = log_create("respuesta.log", "RESPUESTA", true, LOG_LEVEL_TRACE);
	log_trace(logger, "Llegó algo papi ");
	int size=0;
	void* msj;

	switch (cod_op) {
		case PUBLISHER:
			log_trace(logger, "Llegó un PUBLISHER");
			msj = recibirMensajeServidor(cliente_fd,&size);
			log_trace(logger, "Payload: %d", size);
			manejarMensaje(msj,logger,size);

			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
	}
}

void serve_client(int* socket)
{
	int cod_op=0;
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
