#include "conexionBroker.h"
#include "cache/dynamicCache.h"

void manejarSuscriptor(void* contenido, int socketCliente){
	int offset = 0;
	int tamanio, numeroActual;
	memcpy(&tamanio, contenido, sizeof(int));
	offset += sizeof(int);

	for(int i = 0; i < tamanio; i++){
		memcpy(&numeroActual, contenido + offset, sizeof(int));
		TipoCola colaASuscribirse = (TipoCola)numeroActual;
		agregarSuscriptor(colaASuscribirse, socketCliente);
		offset += sizeof(int);
	}
}

long generarIDMensaje(){
	char* str = temporal_get_string_time();
	unsigned long hash = 0;
	int c;

	while ((c = *str++))
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}

void manejarPublisher(void* contenido, int socketCliente){
	TipoCola colaAGuardar;
	MensajeEnCola* nuevoMensaje = (MensajeEnCola*)malloc(sizeof(MensajeEnCola));
	MensajeEnCache* mensajeEnCache = (MensajeEnCache*)malloc(sizeof(MensajeEnCache));
	void *buffer, *stream;
	int bytes, tamanio;

	//Creo el nuevo MensajeEnCola y lo agrego a la cola correspondiente.
	memcpy(&colaAGuardar, contenido, sizeof(TipoCola));
	nuevoMensaje->contenido = contenido;
	nuevoMensaje->ID = generarIDMensaje();
	nuevoMensaje->IDCorrelativo = -1;

	agregarMensaje(colaAGuardar, nuevoMensaje);

	//Creo el nuevo MensajeEnCache y lo agrego a la caché.
	mensajeEnCache->ID = nuevoMensaje->ID;
	mensajeEnCache->cola = colaAGuardar;
	mensajeEnCache->suscriptoresEnviados = list_create();
	mensajeEnCache->suscriptoresRecibidos = list_create();

	agregarItem(mensajeEnCache, sizeof(MensajeEnCache), mensajeEnCache->ID);

	//Le devuelvo el id del mensaje y el tipo de cola al cliente.
	tamanio = sizeof(long) + sizeof(TipoCola);
	stream = serializarStreamIdMensajePublisher(nuevoMensaje->ID, colaAGuardar);
	buffer = armarPaqueteYSerializar(ID_MENSAJE, tamanio, stream, &bytes);

	send(socketCliente, buffer, bytes, 0);
}

void manejarACK(void* contenido, int suscriptor){
	long idMensaje;
	MensajeEnCache* mensaje;

	memcpy(&idMensaje, contenido, sizeof(long));
	mensaje = obtenerItem(idMensaje);
	list_add(mensaje->suscriptoresRecibidos, &suscriptor);
}

void processRequest(int codOp, int socketCliente){
	int size;
	void* msg;

	switch (codOp) {
		case SUSCRIBER:
			msg = recibirMensajeServidor(socketCliente, &size);
			manejarSuscriptor(msg, socketCliente);

			break;
		case PUBLISHER:
			msg = recibirMensajeServidor(socketCliente, &size);
			manejarPublisher(msg, socketCliente);

			break;
		case ACK:
			msg = recibirMensajeServidor(socketCliente, &size);
			manejarACK(msg, socketCliente);

			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
	}
}

void serveClient(int* socketCliente){
	int cod_op;

	if(recv(*socketCliente, &cod_op, sizeof(int), MSG_WAITALL) == -1){
		cod_op = -1;
	}

	processRequest(cod_op, *socketCliente);
}

void esperarCliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion;
	int socket_cliente;

	tam_direccion = sizeof(struct sockaddr_in);
	socket_cliente = accept(socket_servidor, (void*)&dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serveClient,&socket_cliente);
	pthread_join(thread, NULL);
}

void* serializarMensajeSuscriptor(MensajeEnCola mensajeEnCola, TipoCola tipoCola){
	MensajeParaSuscriptor mensajeParaSuscriptor;

	mensajeParaSuscriptor.IDMensaje = mensajeEnCola.ID;
	mensajeParaSuscriptor.IDMensajeCorrelativo = mensajeEnCola.IDCorrelativo;
	mensajeParaSuscriptor.cola = tipoCola;
	mensajeParaSuscriptor.sizeContenido = sizeof(mensajeEnCola.contenido);
	mensajeParaSuscriptor.contenido = mensajeEnCola.contenido;

	void* stream =  malloc(sizeof(MensajeParaSuscriptor));
	int offset = 0;

	memcpy(stream + offset,&(mensajeParaSuscriptor.IDMensaje), sizeof(long));
	offset += sizeof(long);

	memcpy(stream + offset,&(mensajeParaSuscriptor.IDMensajeCorrelativo), sizeof(long));
	offset += sizeof(long);

	memcpy(stream + offset,&(mensajeParaSuscriptor.cola), sizeof(TipoCola));
	offset += sizeof(TipoCola);

	memcpy(stream + offset,&(mensajeParaSuscriptor.sizeContenido), sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset,&(mensajeParaSuscriptor.contenido), mensajeParaSuscriptor.sizeContenido);
	offset += mensajeParaSuscriptor.sizeContenido;

	return stream;
}

void enviarMensajesPorCola(TipoCola tipoCola){
	ColaConSuscriptores* cola = obtenerCola(tipoCola);

	for(int i = 0; i < list_size(cola->mensajes); i++){
		MensajeEnCola* mensajeEnCola = (MensajeEnCola*)list_get(cola->mensajes, i);
		MensajeEnCache* mensajeEnCache = (MensajeEnCache*)obtenerItem(mensajeEnCola->ID);

		for(int j = 0; j < list_size(cola->suscriptores); j++){
			int suscriptor = (int)list_get(cola->suscriptores, j);
			int esSuscriptorRecibido = list_contains_int(mensajeEnCache->suscriptoresRecibidos, suscriptor);

			if(esSuscriptorRecibido) continue;

			int bytes;
			void* stream = serializarMensajeSuscriptor(*mensajeEnCola, tipoCola);
			void* paqueteSerializado = armarPaqueteYSerializar(NUEVO_MENSAJE_SUSCRIBER, sizeof(MensajeParaSuscriptor), stream, &bytes);
			free(stream);

			if((send(suscriptor, paqueteSerializado, bytes, 0)) > 0){
				int yaEnviado = list_contains_int(mensajeEnCache->suscriptoresEnviados, suscriptor);

				if(!yaEnviado){
					list_add(mensajeEnCache->suscriptoresEnviados, &suscriptor);
				}
			}
		}
	}
}

void enviarMensajesSuscriptores(){
	int colas[] = {NEW, GET, CATCH, APPEARED, LOCALIZED, CAUGHT};

	for (int i = 0; i < 6; i++){
		enviarMensajesPorCola(colas[i]);
	}
}

void iniciarServidor(char *ip, char* puerto){
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

    while(1){
    	esperarCliente(socket_servidor);
    	enviarMensajesSuscriptores();
    }
}
