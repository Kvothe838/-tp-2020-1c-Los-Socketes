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
#include "ManejoDeBloques/manejoDeArchivos.h"

#include "conexionGameCard.h"

void mandarMensajeABroker(void* datosRecibidos,	TipoModulo cola, long IDCorrelativo, t_log* logger) {
	int conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);

	IDMensajePublisher* respuesta;
	if (enviarPublisherConIDCorrelativo(logger, conexionBroker, GAMECARD, datosRecibidos, cola, IDCorrelativo, &respuesta))
		free(respuesta);
	else
		log_info(logger, "ERROR - No se pudo enviar el mensaje a Broker");



	liberar_conexion_cliente(conexionBroker);
}

void process_request(MensajeParaSuscriptor* mensaje){
	t_log* logger = log_create("pruebaLOCA.log", "CONEXION", true, LOG_LEVEL_INFO);
	NewPokemon* pokemonNew;
	GetPokemon* pokemonGet;
	CatchPokemon* pokemonCatch;

	switch (mensaje->cola)
	{
		case NEW:

			pokemonNew = deserializarNew(mensaje->contenido);

			log_info(logger, "LLEGÓ NEW POKEMON CON NOMBRE: %s en la pos %d-%d y son %d", pokemonNew->nombre, pokemonNew->posX, pokemonNew->posY, pokemonNew->cantidad);

			administrarNewPokemon(pokemonNew->nombre, pokemonNew->posX, pokemonNew->posY, pokemonNew->cantidad);

			AppearedPokemon* dataAppeared = getAppearedPokemon(pokemonNew->nombre, pokemonNew->posX, pokemonNew->posY);

			mandarMensajeABroker(dataAppeared, APPEARED, mensaje->ID, logger);

			//free(dataAppeared->nombre);
			free(dataAppeared);

			free(pokemonNew);

			break;

		case GET:

			pokemonGet = deserializarGet(mensaje->contenido);

			log_info(logger, "LLEGÓ GET POKEMON CON NOMBRE: %s", pokemonGet->nombre);

			LocalizedPokemon * dataLocalized = administrarGetPokemon(pokemonGet->nombre);

			printf("Nombre completo: %s\n", dataLocalized->nombre);
			printf("Larog del nombre: %d\n", dataLocalized->largoNombre);
			printf("Posiciones en el mapa: %d\n", dataLocalized->cantidadDeParesDePosiciones);
			printf("Cantidad de elementos en la lista %d\n", list_size(dataLocalized->posiciones));


			//Esto está solamente para poder confirmar que el contenido de la lista está bien
			uint32_t *data;
			uint32_t ciclos = dataLocalized->cantidadDeParesDePosiciones, X = 0, Y = 1;
			while(ciclos != 0){
				ciclos--;

				data = list_get(dataLocalized->posiciones, X);
				log_info(logger, "X:%d", *data);
				data = list_get(dataLocalized->posiciones, Y);
				log_info(logger, "Y:%d", *data);

				X += 2;
				Y += 2;


			}

			//Acá terminó de leer el contenido y empiezo a mandar el mensaje

			mandarMensajeABroker(dataLocalized, LOCALIZED, mensaje->ID, logger);

			//free(dataLocalized->nombre);
			list_destroy(dataLocalized->posiciones);
			free(dataLocalized);

			free(pokemonGet);
			break;

		case CATCH:

			pokemonCatch = deserializarCatch(mensaje->contenido);

			log_info(logger, "LLEGÓ CATCH POKEMON CON NOMBRE: %s en la pos %d-%d", pokemonCatch->nombre, pokemonCatch->posX, pokemonCatch->posY);

			uint32_t resultado = administrarCatchPokemon(pokemonCatch->nombre, pokemonCatch->posX, pokemonCatch->posY);

			CaughtPokemon* dataCaught = getCaughtPokemon(resultado);

			mandarMensajeABroker(dataCaught, CAUGHT, mensaje->ID, logger);

			free(dataCaught);

			free(pokemonCatch);

			break;
		default:
			pthread_exit(NULL);
			break;
	}

	free(mensaje);
}


void atenderMensajeGameboy(int* socket)
{
	OpCode codigo;

	if(recv(*socket, &codigo, sizeof(OpCode), MSG_WAITALL) == -1)
		codigo = -1;

	if(codigo == NUEVO_MENSAJE_SUSCRIBER)
	{
		t_log* logger = log_create("nuevoMensajeSuscriber.log", "Nuevo mensaje suscriber", true, LOG_LEVEL_INFO);
		MensajeParaSuscriptor* mensaje = NULL;
		int recepcionExitosa = recibirMensajeSuscriber(*socket, logger, TEAM, &mensaje, ipBroker, puertoBroker);

		if(!recepcionExitosa) return;

		pthread_create(&threadGameBoy, NULL, (void*)process_request, mensaje);
		pthread_detach(thread);

	}
}

void iniciar_servidor(t_config* config, int socketBroker)
{
	int socketActual = socketBroker;
	int resultado;
    while(1){
    	/*int conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);
    	int suscripcionEnviada = enviarSuscripcion(conexionBroker, GAMECARD, 3, NEW, GET, CATCH);*/


    	OpCode codigo;

    	resultado = recv(socketActual, &codigo, sizeof(OpCode), MSG_WAITALL);
		if(resultado != 0 && resultado != -1 && socketActual != 0){
			if(codigo == NUEVO_MENSAJE_SUSCRIBER)
			{
				t_log* logger = log_create("respuesta.log", "RESPUESTA", true, LOG_LEVEL_TRACE);
				log_trace(logger, "Llegó algo");
				MensajeParaSuscriptor* mensaje = NULL;
				int recepcionExitosa = recibirMensajeSuscriber(socketActual, logger, TEAM, &mensaje, ipBroker, puertoBroker);

				if(!recepcionExitosa) continue;

				pthread_create(&thread,NULL,(void*)process_request,mensaje);
				pthread_detach(thread);
				//process_request(codigo, socketBroker);
			}

		} else{
			t_log* logger = log_create("pruebaLOCA.log", "CONEXION", true, LOG_LEVEL_INFO);

			log_info(logger, "RESULTADO %d", resultado);

			liberar_conexion_cliente(socketActual);
			socketActual = crear_conexion_cliente(ipBroker, puertoBroker);
			if(socketActual != -1)
				enviarSuscripcion(socketActual, GAMECARD, 3, NEW, GET, CATCH);
			if(resultado == 0)
				log_info(logger,"SE CAYÓ BROKER :0");
			if(resultado == -1)
				log_info(logger,"BROKER SIGUE CAIDO D:");
			//volverABUscarSocket(&socketBroker);

			sleep(1);
			log_destroy(logger);
		}

    }
}

void esperarGameBoy(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion;
	int socketCliente;

	tam_direccion = sizeof(struct sockaddr_in);
	socketCliente = accept(socket_servidor, (void*)&dir_cliente, &tam_direccion);

	if(socketCliente == -1) return;

	pthread_create(&thread,NULL,(void*)atenderMensajeGameboy,&socketCliente);
	pthread_join(thread, NULL);

}

void iniciarServidorGameboy(IniciarServidorArgs* argumentos){
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

        int one = 1;
        setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)); //ESTO ES ESENCIAL.

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1){
    	esperarGameBoy(socket_servidor);
    }

    liberar_conexion_cliente(socket_servidor);
}


