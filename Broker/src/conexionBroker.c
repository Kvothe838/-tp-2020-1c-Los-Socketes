#include "conexionBroker.h"
#include "cache/dynamicCache.h"

sem_t mutexNuevoMensaje;
int seguirEjecutando = 1;

void inicializarMutex()
{
	sem_init(&mutexNuevoMensaje, 0, 1);
}

t_list* recibirSuscripcion(int socket, int* cantidadColas)
{
	t_list* colas = list_create();

	recv(socket, cantidadColas, sizeof(int), MSG_WAITALL);

	for(int i = 0; i < *cantidadColas; i++){
		TipoCola* nuevaCola = (TipoCola*)malloc(sizeof(TipoCola));

		recv(socket, nuevaCola, sizeof(TipoCola), MSG_WAITALL);
		list_add(colas, nuevaCola);
	}

	return colas;
}

void manejarSuscripcion(t_list* colas, int cantidadColas, Suscriptor* suscriptor)
{
	for(int i = 0; i < cantidadColas; i++)
	{
		TipoCola* cola = list_get(colas, i);
		agregarSuscriptor(*cola, suscriptor);

		//Log obligatorio.
		log_info(logger, "Suscripción de proceso a cola %s.", tipoColaToString(*cola));
	}
}

long* generarIDMensaje(){
	char* str = temporal_get_string_time();
	long* hash = malloc(sizeof(long));
	*hash = 0;
	int c;

	while ((c = *str++))
		*hash = c + (*hash << 6) + (*hash << 16) - *hash;

	//free(str);

	return hash;
}

Publicacion* recibirPublisher(int socket)
{
	Publicacion* publicacion;
	int tamanio;
	long IDCorrelativo;
	TipoCola cola;
	void* dato;

	recv(socket, &IDCorrelativo, sizeof(long), MSG_WAITALL);
	recv(socket, &cola, sizeof(TipoCola), MSG_WAITALL);
	recv(socket, &tamanio, sizeof(int), MSG_WAITALL);

	dato = malloc(tamanio);
	recv(socket, dato, tamanio, MSG_WAITALL);

	publicacion = (Publicacion*)malloc(sizeof(long) + sizeof(TipoCola) + sizeof(int) + tamanio);

	publicacion->IDCorrelativo = IDCorrelativo;
	publicacion->cola = cola;
	publicacion->tamanioDato = tamanio;
	publicacion->dato = dato;

	return publicacion;
}

void manejarPublisher(int socketCliente){
	void *buffer, *stream;
	int bytes, tamanio;
	long* ID;

	ID = generarIDMensaje();

	Publicacion* publicacion = recibirPublisher(socketCliente);

	//Log obligatorio.
	log_info(logger, "Llegada de nuevo mensaje con ID %ld a cola %s", *ID, tipoColaToString(publicacion->cola));

	//Creo el nuevo MensajeEnCola y lo agrego a la cola correspondiente.

	agregarMensaje(publicacion->cola, ID);
	agregarItem(publicacion->dato, publicacion->tamanioDato, *ID, publicacion->IDCorrelativo, publicacion->cola);

	//Log obligatorio.
	log_info(logger, "Almacenado mensaje con ID %ld y posición de inicio de partición %d.", *ID, obtenerPosicionPorID(*ID));

	//Le devuelvo el id del mensaje y el tipo de cola al cliente.
	stream = serializarStreamIdMensajePublisher(ID, &(publicacion->cola), &tamanio);
	buffer = armarPaqueteYSerializar(ID_MENSAJE, tamanio, stream, &bytes);

	if(send(socketCliente, buffer, bytes, 0) == -1){
		log_info(logger, "Error intentando enviar ID del mensaje al publisher.");
	}

	free(publicacion->dato);
	free(publicacion);
	free(stream);
	free(buffer);
}

/*Ack recibirACK(int socket)
{
	Ack contenido;

	recv(socket, &(contenido.IDMensaje), sizeof(long), MSG_WAITALL);

	return contenido;
}*/

int recibirAck(int socket, Ack** respuesta)
{
	long IDMensaje;

	if(recv(socket, &IDMensaje, sizeof(long), MSG_WAITALL) == -1)
	{
		return 0;
	}

	*respuesta = malloc(sizeof(Ack));
	(*respuesta)->IDMensaje = IDMensaje;

	return 1;
}

void manejarACK(Ack* contenido, Suscriptor* suscriptor, long* IDMensaje){
	//Log obligatorio.
	log_info(logger, "Recepción del mensaje con ID %ld.", contenido->IDMensaje);

	agregarSuscriptorRecibido(contenido->IDMensaje, suscriptor);
}

void processRequest(int opCode, Suscriptor* suscriptor){
	if(opCode <= 0){
		pthread_exit(NULL);
		return;
	}

	switch ((OpCode)opCode) {
		case SUSCRIBER:;
			int cantidadColas;
			t_list* colas = recibirSuscripcion(suscriptor->socket, &cantidadColas);
			manejarSuscripcion(colas, cantidadColas, suscriptor);

			break;
		case PUBLISHER:;
			//sem_wait(&mutexNuevoMensaje); acá rompe porque, seguramente, en algún lado te olvidás de hacer un post xDXDxDXD
			manejarPublisher(suscriptor->socket);
			//sem_post(&mutexNuevoMensaje);

			break;
		/*case ACK:;
			Ack ack = recibirACK(suscriptor->socket);
			manejarACK(ack, suscriptor);

			break;*/
		default:
			break;
	}
}

void serveClient(int* socketCliente){
	Suscriptor* suscriptor = (Suscriptor*)malloc(sizeof(Suscriptor)); //No es necesariamente un suscriptor.
	TipoModulo modulo;
	OpCode opCode;

	if(recv(*socketCliente, &opCode, sizeof(OpCode), MSG_WAITALL) == -1){
		opCode = -1;
	}

	if(recv(*socketCliente, &modulo, sizeof(TipoModulo), MSG_WAITALL) == -1){
		modulo = -1;
	}

	//Log obligatorio.
	log_info(logger, "Conexión de proceso %s a Broker.", tipoModuloToString(modulo));

	suscriptor->socket = *socketCliente;
	suscriptor->modulo = modulo;
	suscriptor->estaCaido = 0;

	processRequest(opCode, suscriptor);

	if(opCode != SUSCRIBER)
	{
		free(suscriptor);
	}
}

void esperarCliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion;
	int socketCliente;

	tam_direccion = sizeof(struct sockaddr_in);
	socketCliente = accept(socket_servidor, (void*)&dir_cliente, &tam_direccion);

	if(socketCliente == -1) return;

	pthread_create(&thread,NULL,(void*)serveClient,&socketCliente);
	pthread_join(thread, NULL);

}

void manejarSuscriptorCaido(Suscriptor* suscriptor)
{
	suscriptor->estaCaido = 1;
}

void enviarMensajesPorCola(TipoCola tipoCola){
	ColaConSuscriptores* cola = obtenerCola(tipoCola);

	for(int i = 0; i < list_size(cola->IDMensajes); i++){
		long* IDMensaje = list_get(cola->IDMensajes, i);

		void* mensaje = obtenerItem(*IDMensaje);
		int* tamanioItem = obtenerTamanioItem(*IDMensaje);
		long* IDCorrelativo = obtenerIDCorrelativoItem(*IDMensaje);

		if(mensaje == NULL || tamanioItem == NULL || IDCorrelativo == NULL){
			abort();
		}

		int tamanioCola = list_size(cola->suscriptores);

		if(tamanioCola > 0)
		{
			cambiarLRU(*IDMensaje);
		}

		for(int j = 0; j < tamanioCola; j++)
		{
			Suscriptor* suscriptor = (Suscriptor*)list_get(cola->suscriptores, j);

			if(suscriptor->estaCaido) continue;

			t_list* suscriptoresEnviados = obtenerSuscriptoresEnviados(*IDMensaje);

			if(suscriptoresEnviados != NULL && esSuscriptorEnviado(suscriptoresEnviados, *suscriptor)) continue;

			int bytes, bytesMensajeSuscriptor;
			void* stream = serializarMensajeSuscriptor(*IDMensaje, *IDCorrelativo, mensaje, *tamanioItem, tipoCola, &bytesMensajeSuscriptor);
			void* paqueteSerializado = armarPaqueteYSerializar(NUEVO_MENSAJE_SUSCRIBER, bytesMensajeSuscriptor, stream, &bytes);
			free(stream);

			if((send(suscriptor->socket, paqueteSerializado, bytes, MSG_NOSIGNAL)) > 0){
				Ack* respuesta;

				int recibidoExitoso = recibirAck(suscriptor->socket, &respuesta);

				if(recibidoExitoso)
				{
					if(respuesta->IDMensaje == *IDMensaje)
					{
						agregarSuscriptorEnviado(*IDMensaje, &suscriptor);

						//Log obligatorio.
						log_info(logger, "Envío de mensaje con ID %ld a suscriptor %s", *IDMensaje, tipoModuloToString(suscriptor->modulo));

						manejarACK(respuesta, suscriptor, IDMensaje);
						free(respuesta);
					} else {
						manejarSuscriptorCaido(suscriptor);
					}
				}
				else
				{
					manejarSuscriptorCaido(suscriptor);
				}
			}
		}

		free(mensaje);
	}
}

void enviarMensajesSuscriptores(){
	while(seguirEjecutando){
		int colas[] = {NEW, GET, CATCH, APPEARED, LOCALIZED, CAUGHT};

		for (int i = 0; i < 6; i++){
			sem_wait(&mutexNuevoMensaje);
			enviarMensajesPorCola(colas[i]);
			sem_post(&mutexNuevoMensaje);
		}
	}
}

void iniciarServidor(IniciarServidorArgs* argumentos){
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(argumentos->ip, argumentos->puerto, &hints, &servinfo);

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

    while(seguirEjecutando){
    	esperarCliente(socket_servidor);
    }
}

void cortarPrograma(){
	seguirEjecutando = 0;
}
