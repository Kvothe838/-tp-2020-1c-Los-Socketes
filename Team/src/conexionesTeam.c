#include "conexionesTeam.h"

t_log* logConexiones;

int enviarGet(char* nombre){
	int exito = 1;
	GetPokemon* pokemon = getGetPokemon(nombre);
	int conexionBroker2 = crear_conexion_cliente(configTeam.ip, configTeam.puerto);
	IDMensajePublisher* respuesta;

	if(!enviarPublisherSinIDCorrelativo(logConexiones,conexionBroker2, configTeam.ID, pokemon, GET, &respuesta)){
		log_info(logConexiones, "NO SE PUDO MANDAR EL GET(%s)",nombre);
		exito = 0;
	} else{
		log_info(logConexiones, "SE PUDO MANDAR EL GET(%s)",nombre);
	}

	log_info(logConexiones, "RESPUESTA: %ld", respuesta->IDMensaje);
	liberar_conexion_cliente(conexionBroker2);

	return exito;
}

int loNecesitoGlobalmenteAppeared(char* nombrePokemon){ // 1 -> LO METO EN EL MAPA, 0 -> LO METO EN COLA RESERVA, (-1) -> ES RECHAZADO
	int loNecesito=(-1);
	pthread_mutex_lock(&acceder_objetivos_globales);
	for(int i=0;i<list_size(OBJETIVO_GLOBAL);i++){
		if(strcmp(getObj(list_get(OBJETIVO_GLOBAL,i))->especie,nombrePokemon)==0){ // ES ACEPTADO PARA EL MAPA O PARA LA RESERVA
			if(getObj(list_get(OBJETIVO_GLOBAL,i))->cantidad > 0){ // LO METO EN EL MAPA
				getObj(list_get(OBJETIVO_GLOBAL,i))->aceptarMas = 0;
				loNecesito=1;
			}else{ // LO METO EN COLA RESERVA
				loNecesito=0;
			}
		}
	}
	pthread_mutex_unlock(&acceder_objetivos_globales);
	log_info(logConexiones,"SALGO DE loNecesitoGlobalmenteAppeared");
	return loNecesito;
}

int loNecesitoGlobalmente(char* nombrePokemon){
	int necesidadGlobal = 0;
	pthread_mutex_lock(&acceder_objetivos_globales);
	for(int i=0;i<list_size(OBJETIVO_GLOBAL);i++){
		if((strcmp(getObj(list_get(OBJETIVO_GLOBAL,i))->especie,nombrePokemon)==0)&&(getObj(list_get(OBJETIVO_GLOBAL,i))->aceptarMas)){
			necesidadGlobal=getObj(list_get(OBJETIVO_GLOBAL,i))->cantidad;
			getObj(list_get(OBJETIVO_GLOBAL,i))->aceptarMas = 0;
		}
	}
	pthread_mutex_unlock(&acceder_objetivos_globales);
	return necesidadGlobal;
}

float distanciaMinimaHaciaAlgunEntrenador(Pokemon* pokemon){
	pthread_mutex_lock(&modificar_cola_disponibles);
	float masCerca = distancia(get_posicion((list_get(DISPONIBLES->elements,0)),0),get_posicion((list_get(DISPONIBLES->elements,0)),1),pokemon->x,pokemon->y);
	for(int i=0;i<queue_size(DISPONIBLES);i++){
		if((masCerca) > distancia(get_posicion((list_get(DISPONIBLES->elements,i)),0),get_posicion((list_get(DISPONIBLES->elements,i)),1),pokemon->x,pokemon->y)){
			(masCerca) = distancia(get_posicion((list_get(DISPONIBLES->elements,i)),0),get_posicion((list_get(DISPONIBLES->elements,i)),1),pokemon->x,pokemon->y);
		}
	}
	pthread_mutex_unlock(&modificar_cola_disponibles);
	log_info(logConexiones,"LA DISTANCIA MINIMA ES: %.2f",(masCerca));
	return masCerca;
}

void reservarPokemon(Pokemon* pokemon){
	pthread_mutex_lock(&modificar_cola_reservas);
	queue_push(POKEMONS_RESERVA,pokemon);
	pthread_mutex_unlock(&modificar_cola_reservas);
	log_info(logConexiones,"POKEMON (%s,%d,%d) RESERVADO EN MEMORIA",pokemon->nombre,pokemon->x,pokemon->y);
}

void filtrarPokemonsLocalized(t_list* pokemonsLocalized, int cantidadNecesidadGlobal){// RECIBO MAS DE LOS QUE NECESITO GLOBALMENTE
	float distanciasMinimasDePokemons[list_size(pokemonsLocalized)];

	for(int i=0;i<list_size(pokemonsLocalized);i++){
		//distanciasMinimasDePokemons[i] = 0;
		distanciasMinimasDePokemons[i] = distanciaMinimaHaciaAlgunEntrenador(list_get(pokemonsLocalized,i));
		log_info(logConexiones,"%f",distanciasMinimasDePokemons[i]);
	}
	//t_list* pokemonesParaAgregar = list_create();
	int iteraciones=0;
	int indiceVariable;int soyMenorATodos;
	int i=0;
	while(i<list_size(pokemonsLocalized)){
		soyMenorATodos=1;
		if((iteraciones < cantidadNecesidadGlobal)&&(distanciasMinimasDePokemons[i]>=0)){
			for(indiceVariable=0;indiceVariable<list_size(pokemonsLocalized);indiceVariable++){
				log_info(logConexiones,"VOY A COMPARAR %f CON %f",distanciasMinimasDePokemons[i],distanciasMinimasDePokemons[indiceVariable]);
				if((soyMenorATodos)&&
					(distanciasMinimasDePokemons[indiceVariable]>=0)&&
					(distanciasMinimasDePokemons[i]>=0)&&
					(distanciasMinimasDePokemons[indiceVariable] < distanciasMinimasDePokemons[i])&&
					(indiceVariable != i)){
					soyMenorATodos=0;
				}
			}
			if(soyMenorATodos){
				log_info(logConexiones,"VOY A AGREGAR A %f AL MAPA PROXIMAMENTE",distanciasMinimasDePokemons[i]);
				iteraciones++;
				distanciasMinimasDePokemons[i] = (-1);
				//list_add(pokemonesParaAgregar,list_get(pokemonsLocalized,i));
				i=(-1);
			}
		}
		i++;
	}
	// RESERVAR POKEMONS
	for(int i=0;i<list_size(pokemonsLocalized);i++){
		if(distanciasMinimasDePokemons[i]!=(-1)){
			reservarPokemon(list_get(pokemonsLocalized,i));
		}else{
			agregar_pokemon_cola(list_get(pokemonsLocalized,i));
			entrenador_mas_cercano(queue_peek(POKEMONS));
		}
	}
	//planificarPokemonsLocalized(pokemonesParaAgregar);
}

void manejarNuevoMensajeSuscriptor(MensajeParaSuscriptor* mensaje)
{
	switch(mensaje->cola)
	{
		case LOCALIZED:;
			LocalizedPokemon* localized = deserializarLocalized(mensaje->contenido);
			log_info(logConexiones,"LLEGO UN LOCALIZED(%s) con %d pares",localized->nombre, localized->cantidadDeParesDePosiciones);
			int cantidadNecesidadGlobal = loNecesitoGlobalmente(localized->nombre);
			if(cantidadNecesidadGlobal==0){
				log_info(logConexiones,"LOCALIZED RECHAZADO");
				free(localized);
				break;
			}

			if(localized->cantidadDeParesDePosiciones <= 0){
				log_info(logConexiones,"EL LOCALIZED NO TIENE PARES");
				free(localized);
				break;
			}

			t_list* pokemonsLocalized = list_create();

			int i = 0;

			while(i < list_size(localized->posiciones)){
				Pokemon* nuevo;
				nuevo = crearPokemon(localized->nombre,*(uint32_t*)list_get(localized->posiciones, i),*(uint32_t*)list_get(localized->posiciones, i+1));
				log_info(logConexiones,"SE CREO UN POKEMON %s PARA LA POSICION (%d,%d)",nuevo->nombre,nuevo->x,nuevo->y);
				list_add(pokemonsLocalized,nuevo);
				i += 2;
			}

			free(localized);

			if(list_size(pokemonsLocalized) <= cantidadNecesidadGlobal){ // AGREGO TODOS LOS RECIBIDOS AL MAPA
				for(int i=0;i<list_size(pokemonsLocalized);i++){
					agregar_pokemon_cola(list_get(pokemonsLocalized,i));
					entrenador_mas_cercano(queue_peek(POKEMONS));
				}


				//planificarPokemonsLocalized(pokemonsLocalized);
			}else{ // RECIBO MAS DE LOS QUE NECESITO, VOY A TENER POKEMONS DE RESERVA
				filtrarPokemonsLocalized(pokemonsLocalized,cantidadNecesidadGlobal);
			}

			break;

		case APPEARED:;
			AppearedPokemon* appeared = deserializarAppeared(mensaje->contenido);
			log_info(logConexiones,"LLEGO UN APPEARED(%s) EN (%d,%d)", appeared->nombre, appeared->posX, appeared->posY);
			int necesidad = loNecesitoGlobalmenteAppeared(appeared->nombre);
			if(necesidad != (-1)){
				Pokemon* nuevo;
				//nuevo = crearPokemon(appeared->nombre,*(uint32_t*)(appeared->posX),*(uint32_t*)(appeared->posY));
				nuevo = crearPokemon(appeared->nombre,(appeared->posX),(appeared->posY));
				if(necesidad==1){
					agregar_pokemon_cola(nuevo);
					entrenador_mas_cercano(queue_peek(POKEMONS));
				}else{
					reservarPokemon(nuevo);
				}
			}else{
				log_info(logConexiones,"APPEARED(%s) EN (%d,%d) RECHAZADO", appeared->nombre, appeared->posX, appeared->posY);
			}

			free(appeared);

			break;

		case CAUGHT:;
			CaughtPokemon* caught = deserializarCaught(mensaje->contenido);
			log_info(logConexiones,"LLEGO UN CAUGHT(%d) PARA EL ID CORRELATIVO %ld", caught->loAtrapo, mensaje->IDMensajeCorrelativo);

			if(consultaGlobal.id_correlativo == mensaje->IDMensajeCorrelativo){
				consultaGlobal.respuesta = caught->loAtrapo;
				consultaGlobal.id_correlativo = 0;
				sem_post(&consultaCatch);
			}

			free(caught);
			break;

		default:
			log_info(logConexiones, "LA CAGUÉ");
	}
}

void funcionDeMierda(){
	int sigo;int indice;
	//logger = iniciar_logger("PRUEBA2", "TEAM");
	log_info(logConexiones,"VOY A MANDAR LOS GETS");
	sigo = 1;
	indice = 0;
	pthread_mutex_lock(&acceder_objetivos_globales);
	while(indice < list_size(OBJETIVO_GLOBAL) && sigo){
		log_info(logConexiones,"VOY A MANDAR UN GET DENTRO DEL WHILE");
		if(!enviarGet(getObj(list_get(OBJETIVO_GLOBAL,indice))->especie)){
			sigo = 0;
		}
		indice++;
	}
	pthread_mutex_unlock(&acceder_objetivos_globales);
	log_info(logConexiones,"YA MANDE LOS GETS");
}

void escucharABroker(){
	int conexionBroker;//int suscripcionEnviada;
	conexionBroker = crear_conexion_cliente(configTeam.ip, configTeam.puerto);
	//suscripcionEnviada =
			enviarSuscripcion(conexionBroker, configTeam.ID, 3, APPEARED, LOCALIZED, CAUGHT);
	//int resultado; ----------------
	while(seguir_abierto_servidor==1){ // EL HILO SERVIDOR DE BROKER SE TERMINA SI SE TERMINA EL HILO PLANIFICACION Y SE PIERDE LA CONEXION CON BROKER
		log_info(logConexiones,"===== VOY A ESCUCHAR AL BROKER =====");
		OpCode codigo;
		//resultado = recv(conexionBroker, &codigo, sizeof(OpCode), MSG_WAITALL); ----------------
		//if(resultado != 0 && resultado != -1 && conexionBroker != 0){ ----------------
		if(conexionBroker != (-1)){
			if( (seguir_abierto_servidor==1) && (recv(conexionBroker, &codigo, sizeof(OpCode), MSG_WAITALL) != 0) ){
				if(codigo == NUEVO_MENSAJE_SUSCRIBER){
					log_info(logConexiones, "LLEGO UN MENSAJE DEL TIPO SUSCRIBER");
					MensajeParaSuscriptor* mensaje = NULL;
					int recepcionExitosa = recibirMensajeSuscriber(conexionBroker, logConexiones, TEAM, &mensaje, ipBroker, puertoBroker);
					if(!recepcionExitosa) continue;
					pthread_create(&thread,NULL,(void*)manejarNuevoMensajeSuscriptor,(mensaje));
					pthread_detach(thread);
				}else{log_info(logConexiones, "LLEGO UN MENSAJE PERO NO DEL TIPO SUSCRIBER");}
			}else{
				//log_info(logConexiones,"conexionBroker: %d",conexionBroker);
				//log_info(logConexiones, "RESULTADO %d, NO HAY CONEXION CON BROKER", resultado);
				liberar_conexion_cliente(conexionBroker);
				log_info(logConexiones,"VOY A ESPERAR UNOS SEGUNDOS PARA VOLVER A RECONECTARME");
				sleep(2);
				conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);
				//suscripcionEnviada =
					enviarSuscripcion(conexionBroker, configTeam.ID, 3, APPEARED, LOCALIZED, CAUGHT);
			}
		}else{
			//log_info(logConexiones,"conexionBroker: %d",conexionBroker);
			//log_info(logConexiones, "RESULTADO %d, NO HAY CONEXION CON BROKER", resultado);
			liberar_conexion_cliente(conexionBroker);
			log_info(logConexiones,"VOY A ESPERAR UNOS SEGUNDOS PARA VOLVER A RECONECTARME");
			sleep(2);
			conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);
			//suscripcionEnviada =
				enviarSuscripcion(conexionBroker, configTeam.ID, 3, APPEARED, LOCALIZED, CAUGHT);
		}
	}
}

void conectarse_broker(){// FUNCION PARA ESCUCHAR A BROKER CONSTANTEMENTE
	funcionDeMierda();
	escucharABroker();
	log_info(logConexiones,"pthread_exit(conectarse_broker)");
	//pthread_exit(NULL);
}







void atenderMensaje(int* socket){
	OpCode codigo;
	if(recv(*socket, &codigo, sizeof(OpCode), MSG_WAITALL) == -1)
		codigo = -1;
	if(codigo == NUEVO_MENSAJE_SUSCRIBER){
		t_log* logger = log_create("nuevoMensajeSuscriber.log", "Nuevo mensaje suscriber", true, LOG_LEVEL_INFO);
		MensajeParaSuscriptor* mensaje = NULL;
		int recepcionExitosa = recibirMensajeSuscriber(*socket, logger, TEAM, &mensaje, ipBroker, puertoBroker);

		if(!recepcionExitosa) return;

		pthread_create(&threadMensajeGameBoy, NULL, (void*)manejarNuevoMensajeSuscriptor, mensaje);
		pthread_detach(thread);
	}
}

void esperarConexionGameBoy(int socket){
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion;
	int socketCliente;

	tam_direccion = sizeof(struct sockaddr_in);

	socketCliente = accept(socket, (void*)&dir_cliente, &tam_direccion);

	if(socketCliente == -1) return;

	pthread_create(&thread,NULL,(void*)atenderMensaje,&socketCliente);
	pthread_join(thread, NULL);
}

void iniciarServidorTeam(){
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(argumentos.ip, argumentos.puerto, &hints, &servinfo);

	for (p=servinfo; p != NULL; p = p->ai_next){

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

	while(seguir_abierto_servidor==1){
		esperarConexionGameBoy(socket_servidor);
	}
	log_info(logConexiones,"pthread_exit(iniciarServidorTeam)");
	liberar_conexion_cliente(socket_servidor);
}
















void conexiones(t_log* logger, Entrenador** team){
	getObjetivosGlobales(team);
	printf("\n-----------------------------");
	for(int i=0;i<list_size(OBJETIVO_GLOBAL);i++){
		printf("\n%s -> %d",getObj(list_get(OBJETIVO_GLOBAL,i))->especie,getObj(list_get(OBJETIVO_GLOBAL,i))->cantidad);
	}
	printf("\n-----------------------------\n\n\n");

	logConexiones = iniciar_logger("pruebasConexiones.log", "Conexion");
	pthread_t c_broker;
	pthread_t c_gameboy;

	pthread_create(&c_broker,NULL,(void*)conectarse_broker,NULL);
	pthread_create(&c_gameboy, NULL,(void*)iniciarServidorTeam,NULL);
	//pthread_create(&c_gameboy, NULL,(void*)iniciarServidorTeam,(void*)&argumentos);

	//pthread_create(&c_gameboy,NULL,(void*)conectarse_broker,NULL);
	pthread_join(c_broker,NULL);
	pthread_join(c_gameboy,NULL);
}


