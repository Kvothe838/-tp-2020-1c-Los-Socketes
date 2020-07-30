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

char *ip, *puerto;

void process_request(OpCode codigo, int cliente_fd){
	if(codigo == NUEVO_MENSAJE_SUSCRIBER)
	{
		t_log* logger = log_create("respuesta.log", "RESPUESTA", true, LOG_LEVEL_TRACE);
		log_trace(logger, "Llegó algo");
		MensajeParaSuscriptor* mensaje = NULL;
		int recepcionExitosa = recibirMensajeSuscriber(cliente_fd, logger, TEAM, &mensaje, ip, puerto);

		NewPokemon* pokemonNew;
		GetPokemon* pokemonGet;
		CatchPokemon* pokemonCatch;

		if(recepcionExitosa)
		{
			switch (mensaje->cola)
			{
				case NEW:

					pokemonNew = deserializarNew(mensaje->contenido);

					log_info(logger, "LLEGÓ NEW POKEMON CON NOMBRE: %s en la pos %d-%d y son %d", pokemonNew->nombre, pokemonNew->posX, pokemonNew->posY, pokemonNew->cantidad);

					administrarNewPokemon(pokemonNew->nombre, pokemonNew->posX, pokemonNew->posY, pokemonNew->cantidad);

					free(pokemonNew);

					break;

				case GET:

					pokemonGet = deserializarGet(mensaje->contenido);


					log_info(logger, "LLEGÓ GET POKEMON CON NOMBRE: %s", pokemonGet->nombre);

					LocalizedPokemon * datosRecibidos = administrarGetPokemon(pokemonGet->nombre);

					printf("%s\n", datosRecibidos->nombre);
					printf("%d\n", datosRecibidos->cantidadDeParesDePosiciones);

					uint32_t offset = 0;
					uint32_t *data;
					uint32_t ciclos = datosRecibidos->cantidadDeParesDePosiciones, X = 0, Y = 1;
					while(ciclos != 0){
						ciclos--;

						data = list_get(datosRecibidos->posiciones, X);
						log_info(logger, "X:%d", *data);
						data = list_get(datosRecibidos->posiciones, Y);
						log_info(logger, "Y:%d", *data);

						X += 2;
						Y += 2;


					}

					if(enviarPublisherConIDCorrelativo(cliente_fd, TEAM, datosRecibidos, GET, mensaje->ID))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje");
					}

					OpCode codigoOperacion;

					recv(cliente_fd, &codigoOperacion, sizeof(OpCode), 0);

					if(codigoOperacion == ID_MENSAJE){
						IDMensajePublisher* mensajeBasura = NULL;
						int recibidoExitoso = recibirIDMensajePublisher(cliente_fd, mensajeBasura);
						free(mensajeBasura);
					}

					free(pokemonGet);

					break;

				case CATCH:

					pokemonCatch = deserializarCatch(mensaje->contenido);

					log_info(logger, "LLEGÓ CATCH POKEMON CON NOMBRE: %s en la pos %d-%d", pokemonCatch->nombre, pokemonCatch->posX, pokemonCatch->posY);

					administrarCatchPokemon(pokemonCatch->nombre, pokemonCatch->posX, pokemonCatch->posY);

					free(pokemonCatch);

					break;
				default:
					pthread_exit(NULL);
					break;
			}

			free(mensaje);
		}

	}
	/*if(colaRecibida <= 0){
		pthread_exit(NULL);
	}

	switch (colaRecibida) {*/
		/*case GET:

			log_trace(logger, "Llegó un GET");
			msg = malloc(size);
			recv(cliente_fd, msg, size, MSG_WAITALL);

			GetPokemon* pokemonGet = (GetPokemon*)deserializarGet(msg, (int)size);

			log_trace(logger, "Nombre: %s", pokemonGet->nombre);

			log_trace(logger, "Largo nombre (strlen): %d", strlen(pokemonGet->nombre));

			LocalizedPokemon * datosRecibidos = administrarGetPokemon(pokemonGet->nombre);

			printf("%s\n", datosRecibidos->nombre);
			printf("%d\n", datosRecibidos->cantidadDePosiciones);

			uint32_t offset = 0;
			uint32_t *data;
			uint32_t ciclos = datosRecibidos->cantidadDePosiciones;
			while(ciclos != 0){
				ciclos--;

				memcpy(&data, (datosRecibidos->data + offset), sizeof(uint32_t));
				printf("X:%d - ", (int)data);
				offset += sizeof(uint32_t);
				memcpy(&data, (datosRecibidos->data + offset), sizeof(uint32_t));
				printf("Y:%d - ", (int)data);
				offset += sizeof(uint32_t);
				memcpy(&data, (datosRecibidos->data + offset), sizeof(uint32_t));
				printf("Cantidad:%d\n", (int)data);
				offset += sizeof(uint32_t);
			}

			break;*/
		/*case CATCH:
			log_trace(logger, "Llegó un CATCH");
			msg = malloc(size);
			recv(cliente_fd, msg, size, MSG_WAITALL);

			CatchPokemon* pokemonCatch = (CatchPokemon*)deserializarCatch(msg);
			log_trace(logger, "Nombre: %s", pokemonCatch->nombre);
			log_trace(logger, "X: %d", pokemonCatch->posX);
			log_trace(logger, "Y: %d", pokemonCatch->posY);


			administrarCatchPokemon(pokemonCatch->nombre, pokemonCatch->posX, pokemonCatch->posY);

			break;

		case NEW:
			log_trace(logger, "Llegó un NEW");
			msg = malloc(size);
			recv(cliente_fd, msg, size, MSG_WAITALL);

			NewPokemon* pokemonNew = (NewPokemon*)deserializarNew(msg);
			log_trace(logger, "Nombre: %s", pokemonNew->nombre);
			log_trace(logger, "X: %d", pokemonNew->posX);
			log_trace(logger, "Y: %d", pokemonNew->posY);
			log_trace(logger, "Cantidad: %d", pokemonNew->cantidad);


			administrarNewPokemon(pokemonNew->nombre, pokemonNew->posX, pokemonNew->posY, pokemonNew->cantidad);

			break;
		default:
			pthread_exit(NULL);
	}*/
}


void serve_client(int* socket)
{
	OpCode codigo;
	if(recv(*socket, &codigo, sizeof(OpCode), MSG_WAITALL) == -1)
		codigo = -1;
	process_request(codigo, *socket);
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

void iniciar_servidor(t_config* config, int socketBroker)
{
	/*ip = config_get_string_value(config, "IP_GAMECARD");
	puerto = config_get_string_value(config, "PUERTO_GAMECARD");

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

    printf("Socket nro: %d", socket_servidor);*/

    while(1){
    	OpCode codigo;
		if(recv(socketBroker, &codigo, sizeof(OpCode), MSG_WAITALL) == -1)
			codigo = -1;
		process_request(codigo, socketBroker);
    }
    	//esperar_cliente(socket_servidor);

}

