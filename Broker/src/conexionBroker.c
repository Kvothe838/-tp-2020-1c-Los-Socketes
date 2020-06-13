#include "conexionBroker.h"
#include "cache/dynamicCache.h"

void manejarSuscriptor(void* contenido, int socketCliente){
	int offset = 0;
	int tamanio;
	TipoCola colaASuscribirse;
	memcpy(&tamanio, contenido + offset, sizeof(int));
	offset += sizeof(int);

	log_info(logger, "TAMAÑO: %d", tamanio);

	for(int i = 0; i < tamanio; i++){
		memcpy(&colaASuscribirse, contenido + offset, sizeof(TipoCola));
		log_info(logger, "COLA: %s", tipoColaToString(colaASuscribirse));
		agregarSuscriptor(colaASuscribirse, socketCliente);

		//Log obligatorio.
		log_info(logger, "Suscripción de proceso a cola %s.", tipoColaToString(colaASuscribirse));

		offset += sizeof(TipoCola);
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
	int bytes, tamanio, idCorrelativo;

	//Creo el nuevo MensajeEnCola y lo agrego a la cola correspondiente.
	memcpy(&idCorrelativo, contenido, sizeof(long));
	memcpy(&colaAGuardar, contenido + sizeof(long), sizeof(TipoCola));
	nuevoMensaje->contenido = contenido;
	nuevoMensaje->ID = generarIDMensaje();
	nuevoMensaje->IDCorrelativo = -1;

	agregarMensaje(colaAGuardar, nuevoMensaje);

	//Log obligatorio.
	log_info(logger, "Llegada de nuevo mensaje con ID %d a cola %s", nuevoMensaje->ID, tipoColaToString(colaAGuardar));

	//Creo el nuevo MensajeEnCache y lo agrego a la caché.
	mensajeEnCache->ID = nuevoMensaje->ID;
	mensajeEnCache->cola = colaAGuardar;
	mensajeEnCache->suscriptoresEnviados = list_create();
	mensajeEnCache->suscriptoresRecibidos = list_create();

	agregarItem(mensajeEnCache, sizeof(MensajeEnCache), mensajeEnCache->ID);

	//Log obligatorio.
	log_info(logger, "Almacenado mensaje con ID %d y posición de inicio de partición %d.", mensajeEnCache->ID, obtenerPosicionPorID(mensajeEnCache->ID));

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

	//Log obligatorio.
	log_info(logger, "Recepción del mensaje con ID %d.", idMensaje);
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

	//Log obligatorio.
	log_info(logger, "Conexión de proceso %d a Broker.", socket_cliente);

	pthread_create(&thread,NULL,(void*)serveClient,&socket_cliente);
	pthread_join(thread, NULL);
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
				//Log obligatorio.
				log_info(logger, "Envío de mensaje con ID %d a suscriptor %d.", mensajeEnCola->ID, suscriptor);

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
