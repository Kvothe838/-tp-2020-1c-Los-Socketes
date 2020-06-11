#include "teamInit.h"

/* pthread_mutex_t sumar;
 * pthread_mutex_init(&sumar,NULL);
 * pthread_mutex_lock(&sumar);
 * pthread_mutex_unlock(&sumar);
 */

int verificar_deadlock(Entrenador* entrenador){//1 true, 0 false
	int deadlock=0;
	if(list_size(entrenador->objetivosActuales) > 0){
		if(list_size(entrenador->objetivos) == list_size(entrenador->mios)){
			deadlock=1;
		}
	}
	return deadlock;
}

int verificar_finalizacion(Entrenador* entrenador){
	int fin=0;
	if(list_size(entrenador->objetivosActuales)==0){
		fin=1;
	}
	return fin;
}

void actualizar_estado(Entrenador* persona){
	if(list_size(persona->objetivosActuales)==0){
		persona->estado=SALIR;
	}else{
		if(persona->estado!=NUEVO){
			persona->estado=BLOQUEADO;
		}
	}
}

void atrapar(Entrenador* persona){
	// aca se deberia hacer una conexion y preguntar si atrapó o no al pokemon, es bloqueante.
	list_add(persona->mios,persona->intentar_atrapar);
	int i=0;
	int encontrado=0;
	while((i<list_size(persona->objetivosActuales))&&(encontrado==0)){
		if(strcmp(retornarNombreFantasia(list_get(persona->objetivosActuales,i)),retornarNombrePosta(persona->intentar_atrapar))==0){
			encontrado=1;
			list_remove(persona->objetivosActuales,i);
		}
		i++;
	}
	persona->intentar_atrapar = NULL;
}

void moverse(Entrenador* persona){
	//printf("\n[Entrenador %d] X INICIAL: %d, Y INICIAL: %d",persona->idEntrenador,persona->posicion[0],persona->posicion[1]);
	while(persona->movimiento[0]!=0 || persona->movimiento[1]!=0){
		if(persona->movimiento[0]==0){}else{
			if(persona->movimiento[0] < 0){
				persona->movimiento[0] += 1;
				persona->posicion[0] -= 1;
			}else{
				persona->movimiento[0] -= 1;
				persona->posicion[0] += 1;
			}
		}
		if(persona->movimiento[1]==0){}else{
			if(persona->movimiento[1] < 0){
				persona->movimiento[1] += 1;
				persona->posicion[1] -= 1;
			}else{
				persona->movimiento[1] -= 1;
				persona->posicion[1] += 1;
			}
		}
	}
	//printf("\n[Entrenador %d] X FINAL: %d, Y FINAL: %d",persona->idEntrenador,persona->posicion[0],persona->posicion[1]);
}

void ingreso_a_colas_entrenador(Entrenador* persona){
	actualizar_estado(persona);
	switch(persona->estado){
		case NUEVO:
			pthread_mutex_lock(&modificar_cola_disponibles);
			queue_push(DISPONIBLES,persona);
			pthread_mutex_unlock(&modificar_cola_disponibles);
			break;
		case BLOQUEADO:
			if(verificar_deadlock(persona) == 0){
				pthread_mutex_lock(&modificar_cola_disponibles);
				queue_push(DISPONIBLES,persona);
				pthread_mutex_unlock(&modificar_cola_disponibles);

			}
			else{
				pthread_mutex_lock(&modificar_cola_deadlocks);
				queue_push(DEADLOCKS,persona);
				pthread_mutex_unlock(&modificar_cola_deadlocks);
			}
			break;
		case LISTO:
			pthread_mutex_lock(&modificar_cola_preparados);
			queue_push(PREPARADOS,persona);
			pthread_mutex_unlock(&modificar_cola_preparados);
			break;
		default:
			printf("\nentrenador %d finalizo su mision",persona->idEntrenador);
	}
}

void iniciar_entrenador(Entrenador** entrenador){
	while(verificar_finalizacion(*entrenador)==0){
		sem_wait(&((*entrenador)->activador));
		printf("\n%d se mueve %d en el eje X, %d en el eje Y\n",(*entrenador)->idEntrenador,(*entrenador)->movimiento[0],(*entrenador)->movimiento[1]);
		moverse(*entrenador);
		atrapar(*entrenador);
		ingreso_a_colas_entrenador(*entrenador);

		sem_post(&ya_termine);
	}
	pthread_exit(NULL);
}
/*
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------
 */
int get_posicion(Entrenador* persona,int eje){
	if(eje==0){
		return (persona->posicion[0]);
	}else{
		return (persona->posicion[1]);
	}
}

void cargarConfig(Config* conexionConfig){
	t_log* logger;
	t_config* config;
	logger = iniciar_logger("Team.log", "Team");
	config = leer_config("configTeam.config", logger);
	conexionConfig->posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES"); // lista de strings, ultimo elemento nulo
	conexionConfig->pertenecientes = config_get_array_value(config,"POKEMON_ENTRENADORES");
	conexionConfig->objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
	conexionConfig->ip = config_get_string_value(config, "IP_TEAM");
	conexionConfig->puerto = config_get_string_value(config, "PUERTO_TEAM");
}
Pokemon* crearPokemon(char *nombre,int x, int y) {
	Pokemon *new = malloc(sizeof(Pokemon));
    new->nombre = strdup(nombre);
    new->y=y;
    new->x=x;
    return new;
}
PokemonFantasia* crearObjetivo(char *nombre) {
	PokemonFantasia *new = malloc(sizeof(PokemonFantasia));
    new->nombre = strdup(nombre);
    return new;
}

int cant_entrenadores(char** posiciones){
	int cantidad = 0;
	int i=0;
	while(posiciones[i] != NULL){
		cantidad++;
		i++;
	}
	return cantidad;
}
void asignarPosicion(Entrenador* persona,char* posicion){
	char* token = strtok(posicion, "|");
	int eje=0;
	while (token != NULL) {
		persona->posicion[eje] = atoi(token);
	    token = strtok(NULL, "|");
	    eje++;
	}
}
void asignarPertenecientes(Entrenador* persona,char* pokemons){
	char* token = strtok(pokemons, "|");
	persona->mios = list_create();
	while (token != NULL) {
		Pokemon* p;
		p = crearPokemon(token,0,0);
		list_add(persona->mios,p);
		token = strtok(NULL, "|");
	}
}
void asignarObjetivos(Entrenador* persona,char* pokemons){
	char* token = strtok(pokemons, "|");
		persona->objetivos = list_create();
		while (token != NULL) {
			PokemonFantasia* p;
			p = crearObjetivo(token);
			list_add(persona->objetivos,p);
			token = strtok(NULL, "|");
		}
}
void mostrarObjetivosActualesDe(Entrenador* entrenador){
	t_list* listaObjetivos = entrenador->objetivosActuales;
	printf("\nPokemones por encontrar: ");
	for(int i = 0 ; i<list_size(listaObjetivos);i++){
		PokemonFantasia* pokemon = list_get(listaObjetivos,i);
		printf("\n %s",pokemon->nombre);
	}
	printf("\n");
}
void mostrarEstado(Entrenador* entrenador){
	switch(entrenador->estado){
		case NUEVO:
			printf("NUEVO");
		break;
		case LISTO:
			printf("LISTO");
		break;
		case EJECUTANDO:
			printf("EJECUTANDO");
		break;
		case BLOQUEADO:
			printf("BLOQUEADO");
		break;
		case SALIR:
			printf("SALIR");
		break;
		default:
			printf("DEADLOCK");
	}
}

void mostrarEntrenador(Entrenador* entrenador){
	printf("\nEntrenador(%d)",entrenador->idEntrenador);
	printf(" posicion x=%d",entrenador->posicion[0]);
	printf(" posicion y=%d",entrenador->posicion[1]);
	printf(" estado= ");
	mostrarEstado(entrenador);
	printf("\n");
	PokemonFantasia* pokemon;
	for(int i=0;i<list_size(entrenador->mios); i++ ){

		pokemon =list_get(entrenador->mios,i);
		printf(" pokemon[%d]=",i);
		printf("%s",pokemon->nombre );
	}

	mostrarObjetivosActualesDe(entrenador);
}

char* retornarNombrePosta(Pokemon* p){
	return p->nombre;
}
char* retornarNombreFantasia(PokemonFantasia* p){
	return p->nombre;
}

Entrenador* inicializarEntrenador(int id,char* posicion, char* pokePertenecientes , char* pokeObjetivos){
	Entrenador* entrenador = (Entrenador*)malloc(sizeof(Entrenador));
	entrenador->idEntrenador = id;
	asignarPosicion(entrenador,posicion);
	asignarPertenecientes(entrenador,pokePertenecientes);
	asignarObjetivos(entrenador,pokeObjetivos);
	entrenador->estado = NUEVO;
	asignarObjetivosActuales(entrenador);
	if(verificar_deadlock(entrenador)==1){
		entrenador->estado = BLOQUEADO;
	}
	if(verificar_finalizacion(entrenador)==0){
		entrenador->movimiento[0]=0;entrenador->movimiento[1]=0;
		entrenador->intentar_atrapar=NULL;
		sem_init(&(entrenador->activador),0,0);
	}

	mostrarEntrenador(entrenador);
	return entrenador;

}
int getCantEntrenadores(Team team){
	return sizeof(team)-1;
}
void getObjetivosGlobales(Team team){
	for(int i = 0 ;i<getCantEntrenadores(team);i++){
		for(int j = 0 ; j<list_size(team[i]->objetivosActuales);j++){
			list_add(OBJETIVO_GLOBAL,list_get(team[i]->objetivosActuales,j));
		}
	}
}

Team inicializarTeam(char** posiciones, char** pokePertenecientes , char** pokeObjetivos){
	Entrenador** team = (Entrenador**)(malloc(sizeof(Entrenador)));
	OBJETIVO_GLOBAL = list_create();
	PREPARADOS = queue_create();
	DISPONIBLES = queue_create();
	DEADLOCKS = queue_create();
	pthread_mutex_init(&modificar_cola_preparados,NULL);
	pthread_mutex_init(&modificar_cola_disponibles,NULL);
	pthread_mutex_init(&modificar_cola_deadlocks,NULL);
	sem_init(&progreso,0,1);

	pthread_t hilo[sizeof(posiciones) - 1];
	for(int i=0 ; i<sizeof(posiciones) - 1 ; i++){
		team[i] = inicializarEntrenador(i,posiciones[i],pokePertenecientes[i],pokeObjetivos[i]);
		ingreso_a_colas_entrenador(team[i]);
		pthread_create(&hilo[i],NULL,(void*)iniciar_entrenador,&(team[i]));
		pthread_detach(hilo[i]);
	}
	getObjetivosGlobales(team);

	return team;
}

void asignarObjetivosActuales(Entrenador* persona){
	persona->objetivosActuales = list_create();
	t_list* nombresObjetivos = list_map(persona->objetivos,(void*) retornarNombreFantasia);
	t_list* nombresPertenecientes = list_map(persona->mios,(void*) retornarNombrePosta);
	int indiceObjetivos=0;
	int indicePertenecientes;
	int repetido;
	int noRepetido;
	while(indiceObjetivos<list_size(nombresObjetivos)){
		indicePertenecientes=0;
		repetido=1;
		noRepetido=0;
		while((indicePertenecientes<list_size(nombresPertenecientes))&&(repetido)){
			if(!strcmp(list_get(nombresObjetivos,indiceObjetivos),list_get(nombresPertenecientes,indicePertenecientes))){
				repetido=0;
				list_remove(nombresObjetivos,indiceObjetivos);
				list_remove(nombresPertenecientes,indicePertenecientes);
				indiceObjetivos--;
			}else{
				noRepetido++;
			}
			indicePertenecientes++;
		}

		indiceObjetivos++;
	}
	if(list_size(nombresObjetivos)>0){
		for(int i=0;i<list_size(nombresObjetivos);i++){
			PokemonFantasia* p;
			p = crearObjetivo(list_get(nombresObjetivos,i));
			list_add(persona->objetivosActuales,p);
		}
	}
}
Entrenador* getEntrenador(int id, Team team){
	return team[id];
}

void liberarTeam(Team team){
	for(int i=0 ; i< getCantEntrenadores(team);i++){
		free(team[i]);
	}
	free(team);
}
void liberarMemoria(Entrenador **team){
	liberarTeam(team);
}
