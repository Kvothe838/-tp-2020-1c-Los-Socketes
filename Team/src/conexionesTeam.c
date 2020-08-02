#include "conexionesTeam.h"

t_log* logConexiones;

/*
void tratamiento_mensaje(MensajeParaSuscriptor** mensaje){
	if((*mensaje)->cola == APPEARED){
		AppearedPokemon* pokemon = deserializarAppeared((*mensaje)->contenido);
		//Hacés lo que te plazca con el pokemon.
		printf("Llego Appeared con nombre: %s", pokemon->nombre);
		free(pokemon);
	}
	if((*mensaje)->cola == LOCALIZED){
		LocalizedPokemon* pokemon = deserializarLocalized((*mensaje)->contenido);
		printf("\nMe llego un localized para %s, con %d posiciones\n",pokemon->nombre,pokemon->cantidadDeParesDePosiciones);
		free(pokemon);
	}
}
*/
int enviarGet(char* nombre, Config* configTeam){
	//t_log* logger2 = iniciar_logger("loggerBroker2.log", "Broker");
	int exito = 1;
	GetPokemon* pokemon = getGetPokemon(nombre);
	int conexionBroker2 = crear_conexion_cliente((configTeam)->ip, (configTeam)->puerto);
	IDMensajePublisher* respuesta;

	if(!enviarPublisherSinIDCorrelativo(logConexiones,conexionBroker2, configTeam->ID, pokemon, GET, &respuesta)){
		//printf("\nERROR - No se pudo enviar el mensaje GET(%s)\n",nombre); // hacer el caso de retornar 2 ponele, y que se repita el envio
		log_info(logConexiones, "NO SE PUDO MANDAR EL GET(%s)",nombre);
		exito = 0;
	} else{
		//printf("\nSe pudo enviar el mensaje GET(%s)\n",nombre);
		log_info(logConexiones, "SE PUDO MANDAR EL GET(%s)",nombre);
		//printf("\nID Correlativo: %ld\n",respuesta->IDMensaje);
	}

	log_info(logConexiones, "RESPUESTA: %ld", respuesta->IDMensaje);
	//log_destroy(logger2);
	liberar_conexion_cliente(conexionBroker2);

	return exito;
}

void manejarNuevoMensajeSuscriptor(MensajeParaSuscriptor* mensaje)
{
	switch(mensaje->cola)
	{
		case LOCALIZED:;
			LocalizedPokemon* localized = deserializarLocalized(mensaje->contenido);
			log_info(logConexiones,"LLEGO UN LOCALIZED(%s) con %d pares",localized->nombre, localized->cantidadDeParesDePosiciones);

			if(localized->cantidadDeParesDePosiciones <= 0){
				log_info(logConexiones,"EL LOCALIZED NO TIENE PARES");
				free(localized);
				break;
			}

			printf("\nPOSICIONES: ");
			int i = 0;

			while(i < list_size(localized->posiciones)){
				printf("(%d,%d) ", *(uint32_t*)list_get(localized->posiciones, i), *(uint32_t*)list_get(localized->posiciones, i+1));
				i += 2;
			}

			printf("\n");

			free(localized);

			break;

		case APPEARED:;
			AppearedPokemon* appeared = deserializarAppeared(mensaje->contenido);
			log_info(logConexiones,"LLEGO UN APPEARED(%s) en (%d,%d)", appeared->nombre, appeared->posX, appeared->posY);
			free(appeared);

			break;

		case CAUGHT:;
			CaughtPokemon* caught = deserializarCaught(mensaje->contenido);
			free(caught);
			break;

		default:
			log_info(logConexiones, "LA CAGUÉ");
	}
}

void funcionDeMierda(Config* configTeam){
	int conexionBroker;int suscripcionEnviada;
	OpCode codigo;
	conexionBroker = crear_conexion_cliente((configTeam)->ip, (configTeam)->puerto);
	int sigo;int indice;
	suscripcionEnviada = enviarSuscripcion(conexionBroker, (configTeam)->ID, 3, APPEARED, LOCALIZED, CAUGHT);
	logger = iniciar_logger("PRUEBA2", "TEAM");
	if(suscripcionEnviada != -1){
		log_info(logConexiones,"VOY A MANDAR LOS GETS");
			sigo = 1;
			indice = 0;

			while(indice < list_size(OBJETIVO_GLOBAL) && sigo){
				if(!enviarGet(getObj(list_get(OBJETIVO_GLOBAL,indice))->especie, configTeam)){
					sigo = 0;
				}

				int recepcion = recv(conexionBroker, &codigo, sizeof(OpCode), MSG_WAITALL);

				while(recepcion == 0 || recepcion == (-1)){
					log_info(logger, "NO HAY CONEXION CON BROKER");
					sleep(2);

					enviarGet(getObj(list_get(OBJETIVO_GLOBAL,indice))->especie,(configTeam));

					conexionBroker = crear_conexion_cliente((configTeam)->ip, (configTeam)->puerto);

					recepcion = recv(conexionBroker, &codigo, sizeof(OpCode), MSG_WAITALL);
				}

				if(codigo == NUEVO_MENSAJE_SUSCRIBER){
					MensajeParaSuscriptor* mensaje= NULL;
					int recepcionExitosa = recibirMensajeSuscriber(conexionBroker, logger, (configTeam)->ID, &mensaje, (configTeam)->ip, (configTeam)->puerto);
					if(recepcionExitosa){
						log_info(logger, "RECIBI UN MENSAJE PARA TRATAR");
						manejarNuevoMensajeSuscriptor(mensaje);
					}
				}

				indice++;
			}
		}
}

void escucharABroker(Config* configTeam, int socketBroker){
	int suscripcionEnviada;
	int socketActual = socketBroker;
	int resultado;
	while(1){
		log_info(logConexiones,"VOY A ESCUCHAR AL BROKER");
		OpCode codigo;
		resultado = recv(socketActual, &codigo, sizeof(OpCode), MSG_WAITALL);
		if(resultado != 0 && resultado != -1 && socketActual != 0){
			if(codigo == NUEVO_MENSAJE_SUSCRIBER){
				//t_log* logger = log_create("respuesta.log", "RESPUESTA", true, LOG_LEVEL_TRACE);
				log_info(logConexiones, "LLEGO ALGO");
				MensajeParaSuscriptor* mensaje = NULL;
				int recepcionExitosa = recibirMensajeSuscriber(socketActual, logConexiones, TEAM, &mensaje, ipBroker, puertoBroker);

				if(!recepcionExitosa) continue;
				//log_trace(logger, "Llegó algo de la cola %s", tipoColaToString(mensaje->cola));
				pthread_create(&thread,NULL,(void*)manejarNuevoMensajeSuscriptor,(mensaje));
				pthread_detach(thread);
				//process_request(codigo, socketBroker);
			}
		}else{
			log_info(logConexiones, "RESULTADO %d, NO HAY CONEXION CON BROKER", resultado);

			liberar_conexion_cliente(socketActual);
			log_info(logConexiones,"VOY A ESPERAR UNOS SEGUNDOS PARA VOLVER A RECONECTARME");
			sleep(2);
			socketActual = crear_conexion_cliente(ipBroker, puertoBroker);
			suscripcionEnviada = enviarSuscripcion(socketActual, (configTeam)->ID, 3, APPEARED, LOCALIZED, CAUGHT);
			if(socketActual != -1 && suscripcionEnviada != -1){
				funcionDeMierda(configTeam);
				//enviarSuscripcion(socketActual, (configTeam)->ID, 3, APPEARED, LOCALIZED, CAUGHT);
			}
			/*
			if(resultado == 0)
				log_info(logConexiones,"SE CAYÓ BROKER");
			if(resultado == -1)
				log_info(logConexiones,"BROKER SIGUE CAIDO");
			*/
			//volverABUscarSocket(&socketBroker);
			//log_destroy(logger);
		}

	}
}

void conectarse_broker(Config** configTeam){// FUNCION PARA ESCUCHAR A BROKER CONSTANTEMENTE
	funcionDeMierda((*configTeam));
	int conexionBroker;int suscripcionEnviada;
	conexionBroker = crear_conexion_cliente((*configTeam)->ip, (*configTeam)->puerto);
	suscripcionEnviada = enviarSuscripcion(conexionBroker, (*configTeam)->ID, 3, APPEARED, LOCALIZED, CAUGHT);

	escucharABroker((*configTeam),conexionBroker);


	/*while(1){
		int recepcion = recv(conexionBroker, &codigo, sizeof(OpCode), MSG_WAITALL);
		if( recepcion == 0 || recepcion == -1){// NO HAY CONEXION
			printf("\nERROR - Broker no conectado(1)\n");
			sleep(2);
			conexionBroker = crear_conexion_cliente((*configTeam)->ip, (*configTeam)->puerto);
			
			suscripcionEnviada = enviarSuscripcion(conexionBroker, (*configTeam)->ID, 3, APPEARED, LOCALIZED, CAUGHT);
			printf("\nsuscripcionEnviada: %d\n",suscripcionEnviada);
			if(suscripcionEnviada != (-1)){
				sigo=1;
				indice=0;
				while((indice < list_size(OBJETIVO_GLOBAL) && sigo)){
					if(!enviarGet(getObj(list_get(OBJETIVO_GLOBAL,indice))->especie,(*configTeam))){
						sigo=0;
					}
					indice++;
				}
			}
			getObjetivosGlobales(team);
		}else{
			printf("\nRECIBI ALGO PARA TRATAR\n");
			log_info(logger, "LLEGA UN MENSAJE NUEVO");
			if(codigo == NUEVO_MENSAJE_SUSCRIBER){
				log_info(logger, "LLEGA BIEN");
				printf("\nRECIBI UN NUEVO_MENSAJE_SUSCRIBER\n");
				MensajeParaSuscriptor* mensaje= NULL;
				int recepcionExitosa = recibirMensajeSuscriber(conexionBroker, logger, (*configTeam)->ID, &mensaje, (*configTeam)->ip, (*configTeam)->puerto);
				if(recepcionExitosa){

					manejarNuevoMensajeSuscriptor(mensaje);

					pthread_t tratamiento;
					pthread_create(&tratamiento,NULL,(void*)tratamiento_mensaje,&mensaje);
					pthread_detach(tratamiento);

				}
			
			}else{
				printf("\nNO RECIBI UN NUEVO_MENSAJE_SUSCRIBER\n");
			}
			sleep(2);
		}
	}*/
}

void conexiones(Config* configTeam, t_log* logger){
	printf("\n-----------------------------");
	for(int i=0;i<list_size(OBJETIVO_GLOBAL);i++){
		printf("\n%s -> %d",getObj(list_get(OBJETIVO_GLOBAL,i))->especie,getObj(list_get(OBJETIVO_GLOBAL,i))->cantidad);
	}
	printf("\n-----------------------------\n\n\n");

	logConexiones = iniciar_logger("pruebasConexiones.log", "Team");
	pthread_t c_broker;
	//pthread_t c_gameboy;
	pthread_create(&c_broker,NULL,(void*)conectarse_broker,&configTeam);

	//pthread_create(&c_gameboy,NULL,(void*)conectarse_broker,NULL);
	pthread_join(c_broker,NULL);

	//pthread_detach(c_gameboy);
}

void atenderMensaje(int* socket)
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

		pthread_create(&threadMensajeGameBoy, NULL, (void*)manejarNuevoMensajeSuscriptor, mensaje);
		pthread_detach(thread);
	}
}

void esperarConexionGameBoy(int socket)
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion;
	int socketCliente;

	tam_direccion = sizeof(struct sockaddr_in);
	socketCliente = accept(socket, (void*)&dir_cliente, &tam_direccion);

	if(socketCliente == -1) return;

	pthread_create(&thread,NULL,(void*)atenderMensaje,&socketCliente);
	pthread_join(thread, NULL);
}

void iniciarServidorTeam(IniciarServidorArgs* argumentos)
{
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
		esperarConexionGameBoy(socket_servidor);
	}

	liberar_conexion_cliente(socket_servidor);
}

