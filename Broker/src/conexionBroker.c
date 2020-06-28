#include "conexionBroker.h"
#include "cache/dynamicCache.h"

void manejarSuscriptor(void* contenido, int socketCliente){
	int offset = 0;
	int tamanio;
	TipoCola colaASuscribirse;
	TipoModulo modulo;
	Suscriptor suscriptor;

	memcpy(&tamanio, contenido + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&modulo, contenido + offset, sizeof(TipoModulo));
	offset += sizeof(TipoModulo);

	suscriptor.socket = socketCliente;
	suscriptor.modulo = modulo;

	for(int i = 0; i < tamanio; i++){
		memcpy(&colaASuscribirse, contenido + offset, sizeof(TipoCola));
		agregarSuscriptor(colaASuscribirse, suscriptor);

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

void manejarPublisher(void* contenido, int tamanioContenido, int socketCliente){
	TipoCola cola;
	void *buffer, *stream;
	int bytes, tamanio, offset = 0;
	long ID, *IDCorrelativo;

	ID = generarIDMensaje();

	//Creo el nuevo MensajeEnCola y lo agrego a la cola correspondiente.
	memcpy(&IDCorrelativo, contenido + offset, sizeof(long));
	offset += sizeof(long);
	memcpy(&cola, contenido + offset, sizeof(TipoCola));
	offset += sizeof(TipoCola);

	agregarMensaje(cola, ID);

	//Log obligatorio.
	log_info(logger, "Llegada de nuevo mensaje con ID %d a cola %s", ID, tipoColaToString(cola));

	agregarItem(contenido, tamanioContenido, ID, IDCorrelativo, cola);

	//Log obligatorio.
	log_info(logger, "Almacenado mensaje con ID %d y posición de inicio de partición %d.", ID, obtenerPosicionPorID(ID));

	//Le devuelvo el id del mensaje y el tipo de cola al cliente.
	tamanio = sizeof(long) + sizeof(TipoCola);
	stream = serializarStreamIdMensajePublisher(ID, cola);
	buffer = armarPaqueteYSerializar(ID_MENSAJE, BROKER, tamanio, stream, &bytes);

	send(socketCliente, buffer, bytes, 0);
}

void manejarACK(void* contenido, Suscriptor suscriptor){
	long IDMensaje;

	memcpy(&IDMensaje, contenido, sizeof(long));

	//Log obligatorio.
	log_info(logger, "Recepción del mensaje con ID %d.", IDMensaje);

	agregarSuscriptorRecibido(IDMensaje, &suscriptor);
}

void processRequest(int opCode, Suscriptor suscriptor){
	int size;
	void* msg;

	if(opCode <= 0){
		pthread_exit(NULL);
		return;
	}

	msg = recibirMensajeServidor(suscriptor.socket, &size);

	switch ((OpCode)opCode) {
		case SUSCRIBER:
			manejarSuscriptor(msg, suscriptor.socket);

			break;
		case PUBLISHER:
			manejarPublisher(msg, size, suscriptor.socket);

			break;
		case ACK:
			manejarACK(msg, suscriptor);

			break;
		default:
			break;
	}
}

void serveClient(int* socketCliente){
	Suscriptor suscriptor;
	int modulo, opCode;

	if(recv(*socketCliente, &modulo, sizeof(int), MSG_WAITALL) == -1){
		modulo = -1;
	}

	if(recv(*socketCliente, &opCode, sizeof(int), MSG_WAITALL) == -1){
		opCode = -1;
	}

	suscriptor.socket = *socketCliente;
	suscriptor.modulo = (TipoModulo)modulo;

	processRequest(opCode, suscriptor);
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

	for(int i = 0; i < list_size(cola->IDMensajes); i++){
		long* IDMensaje = (long*)list_get(cola->IDMensajes, i);
		void* mensaje = obtenerItem(*IDMensaje);
		int* tamanioItem = obtenerTamanioItem(*IDMensaje);

		for(int j = 0; j < list_size(cola->suscriptores); j++){
			Suscriptor* suscriptor = (Suscriptor*)list_get(cola->suscriptores, j);
			t_list* suscriptoresRecibidos = obtenerSuscriptoresRecibidos(*IDMensaje);

			if(suscriptoresRecibidos == NULL) continue;

			if(esSuscriptorRecibido(suscriptoresRecibidos, *suscriptor)) continue;

			int bytes;
			void* stream = serializarMensajeSuscriptor(*IDMensaje, NULL, mensaje, *tamanioItem, tipoCola);
			void* paqueteSerializado = armarPaqueteYSerializar(NUEVO_MENSAJE_SUSCRIBER, BROKER, sizeof(MensajeParaSuscriptor), stream, &bytes);
			free(stream);

			if((send(suscriptor->socket, paqueteSerializado, bytes, 0)) > 0){
				//Log obligatorio.
				log_info(logger, "Envío de mensaje con ID %d a suscriptor %d.", IDMensaje, suscriptor);

				agregarSuscriptorEnviado(*IDMensaje, suscriptor);
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

void iniciarServidor(IniciarServidorArgs argumentos){
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(argumentos.ip, argumentos.puerto, &hints, &servinfo);

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
    }
}
