#include "conexionTeam.h"
//#include "team.h"
//#include "planificacion.h"
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
#include<shared/serialize.h>

void asignar_movimiento(Entrenador* entrenador,int mov_x,int mov_y,Pokemon* pokemon){
	printf("\nA %d SE LE VA A ASIGNAR %s\n",entrenador->idEntrenador,pokemon->nombre);
	entrenador->intentar_atrapar = pokemon;
	entrenador->movimiento[0]=mov_x;
	entrenador->movimiento[1]=mov_y;
	entrenador->tipoAccion=ATRAPAR;
	entrenador->estado = LISTO;
	pthread_mutex_lock(&modificar_cola_preparados);
	queue_push(PREPARADOS,entrenador);
	pthread_mutex_unlock(&modificar_cola_preparados);
	fprintf(logTP,"Se matcheo al Entrenador %d con %s, que aparecio en (%d,%d)\n",entrenador->idEntrenador,pokemon->nombre,pokemon->x,pokemon->y);
	fprintf(logTP,"Entrenador %d pasa de la cola DISPONIBLES a PREPARADOS: va a intentar atrapar un Pokemon\n",entrenador->idEntrenador);
	//sem_post(&s_ejecucion);
}

float distancia(int e_x,int e_y,int p_x,int p_y){ // se calcula la distancia entre el pokemon recien aparecido y el entrenador en su posicion actual
	int distx= p_x - e_x;
	int disty= p_y - e_y;
	if(distx <0){distx = distx * (-1);}
	if(disty <0){disty = disty * (-1);}
	if(disty==0){return ((float)distx);}
	if(distx==0){return ((float)disty);}
	else{
		float distancia=0.0;
		float suma = (float) ((distx*distx) + (disty*disty));
		while((distancia*distancia)<suma){
			distancia += 0.01;
		}
		return distancia;
	}
}

// -1 DISPONIBLES, -1 POKEMONS, +1 PREPARADOS, -1 PREPARADOS, +1 EJECUTADOS
void entrenador_mas_cercano(Pokemon* pokemon){
	pthread_mutex_lock(&modificar_cola_disponibles);
	int indice_menor=0;int mov_x;int mov_y;
	float masCerca = distancia(get_posicion((list_get(DISPONIBLES->elements,0)),0),get_posicion((list_get(DISPONIBLES->elements,0)),1),pokemon->x,pokemon->y);
	mov_x = pokemon->x - get_posicion((list_get(DISPONIBLES->elements,0)),0);
	mov_y = pokemon->y - get_posicion((list_get(DISPONIBLES->elements,0)),1);
	for(int i=0;i<queue_size(DISPONIBLES);i++){
		//printf("\ndistancia del entrenador %d = %.2f",get_id(list_get(DISPONIBLES->elements,i)),distancia(get_posicion((list_get(DISPONIBLES->elements,i)),0),get_posicion((list_get(DISPONIBLES->elements,i)),1),pokemon->x,pokemon->y));
		if(masCerca > distancia(get_posicion((list_get(DISPONIBLES->elements,i)),0),get_posicion((list_get(DISPONIBLES->elements,i)),1),pokemon->x,pokemon->y)){
			masCerca = distancia(get_posicion((list_get(DISPONIBLES->elements,i)),0),get_posicion((list_get(DISPONIBLES->elements,i)),1),pokemon->x,pokemon->y);
			indice_menor=i;
			mov_x = pokemon->x - get_posicion((list_get(DISPONIBLES->elements,i)),0);
			mov_y = pokemon->y - get_posicion((list_get(DISPONIBLES->elements,i)),1);
		}
	}
	//printf("\n%d atrapo a %s",get_id(list_get(DISPONIBLES->elements,indice_menor)),pokemon->nombre);
	asignar_movimiento(list_get(DISPONIBLES->elements,indice_menor),mov_x,mov_y,pokemon);

	list_remove(DISPONIBLES->elements,indice_menor);
	pthread_mutex_unlock(&modificar_cola_disponibles);
	pthread_mutex_lock(&modificar_cola_pokemons);
	queue_pop(POKEMONS);
	pthread_mutex_unlock(&modificar_cola_pokemons);
	//sem_wait(&esperar_finalizacion);
	mostrarColas();
}

int loNecesito(char* pokemon){
	int necesito=0;
	for(int x=0;x<list_size(OBJETIVO_GLOBAL);x++){
		if(strcmp(pokemon,retornarNombreFantasia(list_get(OBJETIVO_GLOBAL,x)))==0){
			necesito=1;
		}
	}
	return necesito;
}

void agregar_pokemon_cola(char* pokemon, int x, int y){
	Pokemon* nuevo;
	nuevo = crearPokemon(pokemon,x,y);
	pthread_mutex_lock(&modificar_cola_pokemons);
	queue_push(POKEMONS,nuevo);
	pthread_mutex_unlock(&modificar_cola_pokemons);
	//printf("\n%s agregado a cola de pokemons\n",nuevo->nombre);
	//sem_post(&esperar_pokemons);
}

void* obtenerData(TipoCola cola,void* msj,int size,t_log* logger){
	AppearedPokemon* dataPokemon;
	switch(cola){
	case APPEARED:
		dataPokemon = deserializarDato(&msj,size);
		log_trace(logger,"Nombre: %s",dataPokemon->nombre);
		log_trace(logger,"Pos X : %d",dataPokemon->posX);

		log_trace(logger,"Pos Y : %d",dataPokemon->posY);
		break;
	default:
		break;
	}
}
void manejarMensaje(void* msj, t_log* logger,int size){
	printf("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	Pokemon* pokemon = malloc(sizeof(Pokemon));
	int offset = 0;
	int numeroActual;
	int largoNombre;
	TipoCola tipoCola;
	memcpy(&numeroActual, msj + offset, sizeof(int));
	tipoCola = (TipoCola) numeroActual;
	log_trace(logger, "TIPO DE COLA: %s",tipoColaToString(tipoCola));
	offset += sizeof(int);
	memcpy(&(pokemon->x),msj + offset,sizeof(int));
	offset+= sizeof(int);
	log_trace(logger,"X: %d",pokemon->x);
	memcpy(&(pokemon->y),msj + offset,sizeof(int));
	offset+= sizeof(int);
	log_trace(logger,"Y: %d",pokemon->y);
	memcpy(&(largoNombre), msj+offset, sizeof(int));
	offset += sizeof(int);
	char nombre[largoNombre];
	log_trace(logger,"Largo: %d",largoNombre);
	memcpy(nombre, msj+offset , largoNombre +1);
	pokemon->nombre = nombre;
	pokemon->nombre[largoNombre] = '\0';
	log_trace(logger, "Nombre : %s",pokemon->nombre);
	//printf("\n(1)EL POKEMON SE LLAMA: %s\n",pokemon->nombre);
	agregar_pokemon_cola(pokemon->nombre, pokemon->x, pokemon->y);
	entrenador_mas_cercano(queue_peek(POKEMONS));
	sem_post(&hayPreparados);
	free(pokemon);
	//obtenerData(tipoCola,msj,size,logger);
}

void process_request(int cod_op, int cliente_fd){
	//t_log* logger = log_create("respuesta.log", "RESPUESTA", true, LOG_LEVEL_TRACE);
	//log_trace(logger, "Llegó algo papi ");
	int size=0;
	void* msj;

	switch (cod_op) {
		case PUBLISHER:
			log_trace(logger, "Llegó un PUBLISHER");
			msj = recibirMensajeServidor(cliente_fd,&size);
			log_trace(logger, "Payload: %d", size);
			manejarMensaje(msj,logger,size);

			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
	}
}

void serve_client(int* socket)
{
	int cod_op=0;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	process_request(cod_op, *socket);
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

void iniciar_servidor(Config** configTeam)
{
	char* ip = (*configTeam)->ip;
	char* puerto = (*configTeam)->puerto;


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

    while(1)
    	esperar_cliente(socket_servidor);

}
