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

char *ip, *puerto;

void mandarMensajeABroker(void* datosRecibidos,	TipoModulo cola, long IDCorrelativo, t_log* logger) {
	int conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);

	if (!enviarPublisherConIDCorrelativo(conexionBroker, GAMECARD, datosRecibidos, cola, IDCorrelativo))
		log_info(logger, "ERROR - No se pudo enviar el mensaje a Broker");
	else {

		OpCode codigoOperacion;
		recv(conexionBroker, &codigoOperacion, sizeof(OpCode), 0);

		if (codigoOperacion == ID_MENSAJE) {
			log_info(logger, "Llegó un ID_MENSAJE");
			IDMensajePublisher* mensajeBasura = malloc(sizeof(IDMensajePublisher));

			int recibidoExitoso = recibirIDMensajePublisher(conexionBroker,	mensajeBasura);

			if (recibidoExitoso)
				log_info(logger, "Se recibió el id nuevo: %ld",	mensajeBasura->IDMensaje);

			free(mensajeBasura);
		}
	}
	liberar_conexion_cliente(conexionBroker);
}

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

					AppearedPokemon* dataAppeared = getAppearedPokemon(pokemonNew->nombre, pokemonNew->posX, pokemonNew->posY);

					mandarMensajeABroker(dataAppeared, APPEARED, mensaje->ID, logger);

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

					free(pokemonGet);
					break;

				case CATCH:

					pokemonCatch = deserializarCatch(mensaje->contenido);

					log_info(logger, "LLEGÓ CATCH POKEMON CON NOMBRE: %s en la pos %d-%d", pokemonCatch->nombre, pokemonCatch->posX, pokemonCatch->posY);

					uint32_t resultado = administrarCatchPokemon(pokemonCatch->nombre, pokemonCatch->posX, pokemonCatch->posY);

					CaughtPokemon* dataCaught = getCaughtPokemon(resultado);

					mandarMensajeABroker(dataCaught, CAUGHT, mensaje->ID, logger);

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
    while(1){
    	/*int conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);
    	int suscripcionEnviada = enviarSuscripcion(conexionBroker, GAMECARD, 3, NEW, GET, CATCH);*/


    	OpCode codigo;
		if(!(recv(socketBroker, &codigo, sizeof(OpCode), MSG_WAITALL)==-1)){
			process_request(codigo, socketBroker);
			printf("Codigo: %d", codigo);
			if(codigo > NUEVO_MENSAJE_SUSCRIBER || codigo < SUSCRIBER){
				abort();
			}
			continue;
		}

    }
}

