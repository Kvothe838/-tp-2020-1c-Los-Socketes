#include "conexionBroker.h"

void manejarSuscriptor(void* contenido, int socketCliente){
	int offset = 0;
	int tamanio, numeroActual;
	memcpy(&tamanio, contenido, sizeof(int));
	offset += sizeof(int);

	for(int i = 0; i < tamanio; i++){
		memcpy(&numeroActual, contenido + offset, sizeof(int));
		TipoCola colaASuscribirse = (TipoCola)numeroActual;
		agregarSuscriptor(colaASuscribirse, socketCliente);
		log_trace(logger, "Nueva cola agregada: %s",tipoColaToString(colaASuscribirse));
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
	MensajeEnCola nuevoMensaje;
	void* buffer;
	int bytes;

	//Creo el nuevo MensajeEnCola.
	memcpy(&colaAGuardar, contenido, sizeof(TipoCola));
	nuevoMensaje.contenido = contenido;
	nuevoMensaje.ID = generarIDMensaje();
	log_trace(logger, "Mensaje agregado en cola %d", tipoColaToString(colaAGuardar));
	agregarMensaje(colaAGuardar, nuevoMensaje);

	//Le devuelvo el id del mensaje y el tipo de cola al cliente.
	Paquete* paquete = (Paquete*)malloc(sizeof(Paquete));
	paquete->codigoOperacion = ID_MENSAJE;
	paquete->buffer = (Buffer*)malloc(sizeof(long) + sizeof(TipoCola));
	memcpy(paquete->buffer, nuevoMensaje.ID, sizeof(long));
	memcpy(paquete->buffer, colaAGuardar, sizeof(TipoCola));
	buffer = serializar_paquete(paquete, &bytes);
	send(socketCliente, buffer, bytes, 0);
}

void manejarACK(void* contenido, int socketCliente){
	long idMensaje;
	MensajeEnCache mensaje;

	memcpy(&idMensaje, contenido, sizeof(long));
	mensaje = obtenerItem(idMensaje);
}

void processRequest(int codOp, int socketCliente){
	log_trace(logger, "Llegó algo. Cliente: %d", socketCliente);
	int size;
	void* msg;

	switch (codOp) {
		case SUSCRIBER:
			log_trace(logger, "Llegó un Suscriber");
			msg = recibirMensajeServidor(socketCliente, &size);
			log_trace(logger, "Payload: %d", size);
			manejarSuscriptor(msg, socketCliente);

			break;
		case PUBLISHER:
			log_trace(logger, "Llegó un Publisher");
			msg = recibirMensajeServidor(socketCliente, &size);
			log_trace(logger, "Payload: %d", size, socketCliente);
			manejarPublisher(msg);

			break;
		case ACK:
			log_trace(logger, "Llegó un ACK");
			msg = recibirMensajeServidor(socketCliente, &size);
			log_trace(logger, "Payload: %d", size);
			manejarACK(msg, socketCliente);

			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
	}
}

void serveClient(int* socket){
	int cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	processRequest(cod_op, *socket);
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

void administrarMemoria(){

}

void iniciar_servidor(char *ip, char* puerto){
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
    	administrarMemoria();
    }

}
