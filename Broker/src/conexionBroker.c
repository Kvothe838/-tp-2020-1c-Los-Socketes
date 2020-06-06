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
	log_info(logger, "COLA A GUARDAR: %s", tipoColaToString(colaAGuardar));
	nuevoMensaje.contenido = contenido;
	nuevoMensaje.ID = generarIDMensaje();
	log_info(logger, "Mensaje agregado en cola %d", tipoColaToString(colaAGuardar));
	agregarMensaje(colaAGuardar, nuevoMensaje);

	//Le devuelvo el id del mensaje y el tipo de cola al cliente.
	Paquete* paquete = (Paquete*)malloc(sizeof(Paquete));
	paquete->codigoOperacion = ID_MENSAJE;
	paquete->buffer = (Buffer*)malloc(sizeof(long) + sizeof(TipoCola));
	memcpy(paquete->buffer, &nuevoMensaje.ID, sizeof(long));
	memcpy(paquete->buffer, &colaAGuardar, sizeof(TipoCola));
	buffer = serializar_paquete(paquete, &bytes);
	send(socketCliente, buffer, bytes, 0);
	log_info(logger, "ID mensaje enviado: %d", nuevoMensaje.ID);
}

void manejarACK(void* contenido, int suscriptor){
	long idMensaje;
	MensajeEnCache* mensaje;

	memcpy(&idMensaje, contenido, sizeof(long));
	mensaje = obtenerItem(idMensaje);
	list_add(mensaje->suscriptoresRecibidos, &suscriptor);
}

void processRequest(int codOp, int socketCliente){
	log_info(logger, "Llegó algo. Cliente: %d", socketCliente);
	log_info(logger, "COD OP: %d", codOp);
	int size;
	void* msg;

	switch (codOp) {
		case SUSCRIBER:
			log_info(logger, "Llegó un Suscriber");
			msg = recibirMensajeServidor(socketCliente, &size);
			log_info(logger, "Payload: %d", size);
			manejarSuscriptor(msg, socketCliente);

			break;
		case PUBLISHER:
			log_info(logger, "Llegó un Publisher");
			msg = recibirMensajeServidor(socketCliente, &size);
			log_info(logger, "Payload: %d", size, socketCliente);
			manejarPublisher(msg, socketCliente);

			break;
		case ACK:
			log_info(logger, "Llegó un ACK");
			msg = recibirMensajeServidor(socketCliente, &size);
			log_info(logger, "Payload: %d", size);
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
		MensajeEnCola* mensajeEnCola = (MensajeEnCola*)list_get(cola->mensajes, i);//probar si se puede
		MensajeEnCache* mensajeEnCache = (MensajeEnCache*)obtenerItem(mensajeEnCola->ID);

		for(int j = 0; j < list_size(cola->suscriptores); j++){
			int suscriptor = (int)list_get(cola->suscriptores, j);
			int esSuscriptorRecibido = 0;

			for(int k = 0; k < list_size(mensajeEnCache->suscriptoresRecibidos); k++){
				int suscriptorRecibido = (int)list_get(mensajeEnCache->suscriptoresRecibidos, k);

				if(suscriptor == suscriptorRecibido){
					esSuscriptorRecibido = 1;
				}
			}

			if(!esSuscriptorRecibido){
				Paquete* paquete = malloc(sizeof(Paquete));

				paquete->codigoOperacion = NUEVO_MENSAJE_SUSCRIBER;
				paquete->buffer = malloc(sizeof(Buffer));
				paquete->buffer->size = sizeof(MensajeParaSuscriptor);
				paquete->buffer->stream = serializarMensajeSuscriptor(*mensajeEnCola, tipoCola);
				int bytes;
				void* paqueteSerializado = serializar_paquete(paquete, &bytes);
				if((send(suscriptor, paqueteSerializado, bytes, 0)) > 0){
					int yaEnviado = 0;

					for(int k = 0; k < list_size(mensajeEnCache->suscriptoresEnviados); k++){
						int suscriptorEnviado = (int)list_get(mensajeEnCache->suscriptoresEnviados, k);

						if(suscriptor == suscriptorEnviado){
							yaEnviado = 1;
						}
					}

					if(!yaEnviado){
						list_add(mensajeEnCache->suscriptoresEnviados, &suscriptor);
					}
				}
			}
		}
	}
}

void recorrerMemoria(){
	enviarMensajesPorCola(NEW);
	compactarCache();
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
    	/*Escucha a clientes que quieran enviar mensajes y toma la acción correspondiente según opCode.
    	Puede ser un Publisher, un Suscriber o un ACK.*/
    	log_info(logger, "Espero cliente");
    	esperarCliente(socket_servidor);
    	/*Se fija qué mensajes necesitan enviarse a qué suscriptores, luego elimina mensajes que ya
    	no tengan suscriptores y compacta la memoria.*/
    	log_info(logger, "Recorro memoria");
    	recorrerMemoria();
    }

}
