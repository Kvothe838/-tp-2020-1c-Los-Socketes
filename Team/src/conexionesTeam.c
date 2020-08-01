#include "conexionesTeam.h"

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
/*
int enviarGet(char* nombre, int conexionBroker){
	int sigo=1;

	GetPokemon* pokemon = getGetPokemon(nombre);
	int conexionBroker2 = crear_conexion_cliente(ipBroker, puertoBroker);

	return sigo;
}

*/
int enviarGet(char* nombre, Config* configTeam){
	t_log* logger2 = iniciar_logger("loggerBroker2.log", "Broker");
	int sigo=1;
	GetPokemon* pokemon = getGetPokemon(nombre);
	int conexionBroker2 = crear_conexion_cliente((configTeam)->ip, (configTeam)->puerto);
	IDMensajePublisher* respuesta;
	if(!enviarPublisherSinIDCorrelativo(logger2,conexionBroker2, TEAM, pokemon, GET,&respuesta)){
		printf("\nERROR - No se pudo enviar el mensaje GET(%s)\n",nombre); // hacer el caso de retornar 2 ponele, y que se repita el envio
		sigo=0;
	}else{
		printf("\nSe pudo enviar el mensaje GET(%s)\n",nombre);
		printf("\nID Correlativo: %ld\n",respuesta->IDMensaje);
	}
	return sigo;
}

void conectarse_broker(Config** configTeam){ // FUNCION PARA ESCUCHAR A BROKER CONSTANTEMENTE
	int conexionBroker;int suscripcionEnviada;
	OpCode codigo;
	conexionBroker = crear_conexion_cliente((*configTeam)->ip, (*configTeam)->puerto);
	int sigo;int indice;
	suscripcionEnviada = enviarSuscripcion(conexionBroker, (*configTeam)->ID, 3, APPEARED, LOCALIZED, CAUGHT);
	logger = iniciar_logger("PRUEBA2.bin", "TEAM");
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

	while(1){
		int recepcion = recv(conexionBroker, &codigo, sizeof(OpCode), 0);
		if( recepcion == 0 || recepcion == (-1)){// NO HAY CONEXION
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
			//getObjetivosGlobales(team);
		}else{
			printf("\nRECIBI ALGO PARA TRATAR\n");
			if(codigo == NUEVO_MENSAJE_SUSCRIBER){
				printf("\nRECIBI UN NUEVO_MENSAJE_SUSCRIBER\n");
				MensajeParaSuscriptor* mensaje= NULL;
				int recepcionExitosa = recibirMensajeSuscriber(conexionBroker, logger, TEAM, &mensaje, (*configTeam)->ip, (*configTeam)->puerto);
				if(recepcionExitosa){

					if((mensaje)->cola == LOCALIZED){
						LocalizedPokemon* pokemon = deserializarLocalized((mensaje)->contenido);
						printf("\nMe llego un localized para %s, con %d posiciones\n",pokemon->nombre,pokemon->cantidadDeParesDePosiciones);
						free(pokemon);
					}
					/*
					pthread_t tratamiento;
					pthread_create(&tratamiento,NULL,(void*)tratamiento_mensaje,&mensaje);
					pthread_detach(tratamiento);
					*/
				}
			
			}else{
				printf("\nNO RECIBI UN NUEVO_MENSAJE_SUSCRIBER\n");
			}
			sleep(2);
		}
	}
}

void conexiones(Config* configTeam, t_log* logger){
	printf("\n-----------------------------");
	for(int i=0;i<list_size(OBJETIVO_GLOBAL);i++){
		printf("\n%s -> %d",getObj(list_get(OBJETIVO_GLOBAL,i))->especie,getObj(list_get(OBJETIVO_GLOBAL,i))->cantidad);
	}
	printf("\n-----------------------------\n\n\n");
	pthread_t c_broker;
	//pthread_t c_gameboy;
	pthread_create(&c_broker,NULL,(void*)conectarse_broker,&configTeam);

	//pthread_create(&c_gameboy,NULL,(void*)conectarse_broker,NULL);
	pthread_join(c_broker,NULL);

	//pthread_detach(c_gameboy);
}

