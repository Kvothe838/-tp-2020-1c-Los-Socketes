#include "conexionBroker.h"
#include "cache/dynamicCache.h"

t_list* recibirSuscripcion(int socket, int* cantidadColas) {
	t_list* colas = list_create();

	recv(socket, cantidadColas, sizeof(int), MSG_WAITALL);

	for (int i = 0; i < *cantidadColas; i++) {
		TipoCola* nuevaCola = (TipoCola*) malloc(sizeof(TipoCola));

		recv(socket, nuevaCola, sizeof(TipoCola), MSG_WAITALL);
		list_add(colas, nuevaCola);
	}

	return colas;
}

void manejarSuscripcion(t_list* colas, int cantidadColas,
		Suscriptor* suscriptor) {
	for (int i = 0; i < cantidadColas; i++) {
		TipoCola* cola = list_get(colas, i);
		agregarSuscriptor(*cola, suscriptor);

		//Log obligatorio.
		log_info(loggerObligatorio, "Suscripción de proceso a cola %s.",
				tipoColaToString(*cola));
	}
}

long* generarIDMensaje() {
	char* str = temporal_get_string_time();
	long* hash = malloc(sizeof(long));
	*hash = 0;
	int c;

	while ((c = *str++))
		*hash = c + (*hash << 6) + (*hash << 16) - *hash;

	//free(str);

	return hash;
}

Publicacion* recibirPublisher(int socket) {
	Publicacion* publicacion;
	int tamanio;
	long IDCorrelativo;
	TipoCola cola;
	void* dato;
	int recepcion;

	recepcion = recv(socket, &IDCorrelativo, sizeof(long), MSG_WAITALL);

	if (recepcion == 0 || recepcion == -1) {
		log_info(loggerInterno, "Error al recibir publisher");
	}

	recepcion = recv(socket, &cola, sizeof(TipoCola), MSG_WAITALL);

	if (recepcion == 0 || recepcion == -1) {
		log_info(loggerInterno, "Error al recibir publisher");
	}

	recepcion = recv(socket, &tamanio, sizeof(int), MSG_WAITALL);

	if (recepcion == 0 || recepcion == -1) {
		log_info(loggerInterno, "Error al recibir publisher");
	}

	log_info(loggerInterno, "Tamaññño: %d", tamanio);

	dato = malloc(tamanio);
	recepcion = recv(socket, dato, tamanio, MSG_WAITALL);

	if (recepcion == 0 || recepcion == -1) {
		log_info(loggerInterno, "Error al recibir publisher");
	}

	publicacion = (Publicacion*) malloc(
			sizeof(long) + sizeof(TipoCola) + sizeof(int) + tamanio);

	publicacion->IDCorrelativo = IDCorrelativo;
	publicacion->cola = cola;
	publicacion->tamanioDato = tamanio;
	publicacion->dato = dato;

	return publicacion;
}

void manejarPublisher(int socketCliente) {
	void *buffer, *stream;
	int bytes, tamanio;
	long* ID;

	ID = generarIDMensaje();

	Publicacion* publicacion = recibirPublisher(socketCliente);

	//Log obligatorio.
	log_info(loggerObligatorio, "Llegada de nuevo mensaje con ID %ld a cola %s", *ID, tipoColaToString(publicacion->cola));

	//Creo el nuevo MensajeEnCola y lo agrego a la cola correspondiente.

	agregarMensaje(publicacion->cola, ID);
	agregarItemGeneric(publicacion->dato, publicacion->tamanioDato, *ID, publicacion->IDCorrelativo, publicacion->cola);

	int posicion = obtenerPosicionPorIDGeneric(*ID);

	//Log obligatorio.
	log_info(loggerObligatorio,	"Almacenado mensaje con ID %ld y posición de inicio de partición %d.", *ID, posicion);

	//Le devuelvo el id del mensaje y el tipo de cola al cliente.
	stream = serializarStreamIdMensajePublisher(ID, &(publicacion->cola), &tamanio);
	buffer = armarPaqueteYSerializar(ID_MENSAJE, tamanio, stream, &bytes);

	if (send(socketCliente, buffer, bytes, 0) == -1) {
		log_info(loggerInterno,	"Error intentando enviar ID del mensaje al publisher.");
	}

	free(publicacion->dato);
	free(publicacion);
	free(stream);
	free(buffer);
}

void manejarACK(Ack* contenido, Suscriptor* suscriptor) {
	//Log obligatorio.
	log_info(loggerObligatorio, "Recepción del mensaje con ID %ld.",
			contenido->IDMensaje);

	agregarSuscriptorRecibido(contenido->IDMensaje, suscriptor);
}

void processRequest(int opCode, Suscriptor* suscriptor) {
	if (opCode <= 0) {
		pthread_exit(NULL);
		return;
	}

	switch ((OpCode) opCode) {
	case SUSCRIBER:;
		int cantidadColas;
		t_list* colas = recibirSuscripcion(suscriptor->socket, &cantidadColas);
		manejarSuscripcion(colas, cantidadColas, suscriptor);

		break;
	case PUBLISHER:;
		manejarPublisher(suscriptor->socket);

		break;
	default:
		break;
	}

	sem_post(semColaMensajes);
}

void serveClient(int* socketCliente) {
	Suscriptor* suscriptor = (Suscriptor*) malloc(sizeof(Suscriptor)); //No es necesariamente un suscriptor.
	TipoModulo modulo;
	OpCode opCode;

	if (recv(*socketCliente, &opCode, sizeof(OpCode), MSG_WAITALL) == -1) {
		opCode = -1;
	}

	if (recv(*socketCliente, &modulo, sizeof(TipoModulo), MSG_WAITALL) == -1) {
		modulo = -1;
	}

	//Log obligatorio.
	log_info(loggerObligatorio, "Conexión de proceso %s a Broker.",
			tipoModuloToString(modulo));

	suscriptor->socket = *socketCliente;
	suscriptor->modulo = modulo;
	suscriptor->estaCaido = 0;

	processRequest(opCode, suscriptor);

	if (opCode != SUSCRIBER) {
		free(suscriptor);
	}
}

void esperarCliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion;
	int socketCliente;

	tam_direccion = sizeof(struct sockaddr_in);
	socketCliente = accept(socket_servidor, (void*) &dir_cliente,
			&tam_direccion);

	if (socketCliente == -1)
		return;

	pthread_create(&thread, NULL, (void*) serveClient, &socketCliente);
	pthread_join(thread, NULL);
}

void manejarSuscriptorCaido(Suscriptor** suscriptor) {
	(*suscriptor)->estaCaido = 1;
}

void enviarMensajesPorCola(TipoCola tipoCola) {
	ColaConSuscriptores* cola = obtenerCola(tipoCola);

	for (int i = 0; i < list_size(cola->IDMensajes); i++) {
		long* IDMensaje = list_get(cola->IDMensajes, i);
		void* mensaje = obtenerItemGeneric(*IDMensaje);
		int* tamanioItem = obtenerTamanioItemGeneric(*IDMensaje);
		long* IDCorrelativo = obtenerIDCorrelativoItemGeneric(*IDMensaje);

		if (mensaje == NULL || tamanioItem == NULL || IDCorrelativo == NULL) {
			log_info(loggerInterno, "No se obtuvieron datos para IDMensaje %ld",
					*IDMensaje);
			continue;
		}

		for (int j = 0; j < list_size(cola->suscriptores); j++) {
			Suscriptor* suscriptor = (Suscriptor*) list_get(cola->suscriptores,j);
			if (suscriptor->estaCaido)
				continue;

			t_list* suscriptoresEnviados = obtenerSuscriptoresEnviadosGeneric(*IDMensaje);

			if (suscriptoresEnviados != NULL
					&& esSuscriptorEnviado(suscriptoresEnviados, *suscriptor))
				continue;

			int bytes, bytesMensajeSuscriptor;
			void* stream = serializarMensajeSuscriptor(*IDMensaje,
					*IDCorrelativo, mensaje, *tamanioItem, tipoCola,
					&bytesMensajeSuscriptor);
			void* paqueteSerializado = armarPaqueteYSerializar(
					NUEVO_MENSAJE_SUSCRIBER, bytesMensajeSuscriptor, stream,
					&bytes);
			free(stream);

			if ((send(suscriptor->socket, paqueteSerializado, bytes,
					MSG_NOSIGNAL)) <= 0)
				continue;

			Ack* respuesta;

			int recibidoExitoso = recibirAck(suscriptor->socket, &respuesta);

			if (!recibidoExitoso || respuesta->IDMensaje != *IDMensaje) {
				manejarSuscriptorCaido(&suscriptor);
				continue;
			}

			agregarSuscriptorEnviadoGeneric(*IDMensaje, &suscriptor);
			cambiarLRUGeneric(*IDMensaje);

			//Log obligatorio.
			log_info(loggerObligatorio,
					"Envío de mensaje con ID %ld a suscriptor %s", *IDMensaje,
					tipoModuloToString(suscriptor->modulo));

			manejarACK(respuesta, suscriptor);
			free(respuesta);

		}

		if(mensaje != NULL)
			free(mensaje);
	}
}

void enviarMensajesSuscriptores() {
	int colas[] = { NEW, GET, CATCH, APPEARED, LOCALIZED, CAUGHT };
	while (1) {
		sem_wait(semColaMensajes);

		for(int i = 0; i < 6; i++) {
			enviarMensajesPorCola(colas[i]);
		}
	}
}

void iniciarServidor(IniciarServidorArgs* argumentos) {
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(argumentos->ip, argumentos->puerto, &hints, &servinfo);

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1)
			continue;

		int one = 1;
		setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &one,
				sizeof(one)); //ESTO ES ESENCIAL.

		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_servidor);
			continue;
		}
		break;
	}

	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);

	while (1) {
		esperarCliente(socket_servidor);
	}

	liberar_conexion_cliente(socket_servidor);
}
