#include "teamInit.h"
t_log* logInit;
int tiempoRetardo;
char* algoritmo;
void agregar_pokemon_cola(Pokemon* nuevo){
	pthread_mutex_lock(&modificar_cola_pokemons);
	queue_push(POKEMONS,nuevo);
	pthread_mutex_unlock(&modificar_cola_pokemons);
}

void asignar_movimiento(Entrenador* entrenador,int mov_x,int mov_y,Pokemon* pokemon){
	entrenador->intentar_atrapar = pokemon;
	entrenador->movimiento[0]=mov_x;
	entrenador->movimiento[1]=mov_y;
	entrenador->tipoAccion=ATRAPAR;
	entrenador->estado = LISTO;

	pthread_mutex_lock(&modificar_cola_preparados);
	queue_push(PREPARADOS,entrenador);
	pthread_mutex_unlock(&modificar_cola_preparados);
	log_info(logInit,"A %d SE LE VA A ASIGNAR %s",entrenador->idEntrenador,pokemon->nombre);
	log_info(logInit," = %d = PASA A LA COLA PREPARADOS",entrenador->idEntrenador);
	//fprintf(logTP,"Se matcheo al Entrenador %d con %s, que aparecio en (%d,%d)\n",entrenador->idEntrenador,pokemon->nombre,pokemon->x,pokemon->y);
	//fprintf(logTP,"Entrenador %d pasa de la cola DISPONIBLES a PREPARADOS: va a intentar atrapar un Pokemon\n",entrenador->idEntrenador);
	//log_info(logInit,"Se matcheo al Entrenador %d con %s, que aparecio en (%d,%d)\n",entrenador->idEntrenador,pokemon->nombre,pokemon->x,pokemon->y);
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

void entrenador_mas_cercano(Pokemon* pokemon){
	pthread_mutex_lock(&modificar_cola_disponibles);
	if(queue_size(DISPONIBLES) > 0){
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
		//mostrarColas();
		sem_post(&hayPreparados);
	}else{
		pthread_mutex_unlock(&modificar_cola_disponibles);
	}
}

Metrica* getMetrica(int indice){
	return (list_get(METRICAS_ENTRENADORES,indice));
}
int getQuantum(int indice){
	return (getMetrica(indice)->quantumTotal);
}
void sumarQuantum(int indice, int cantSumar){
	getMetrica(indice)->quantumTotal += cantSumar;
}

void mostrarColas(){
	printf("\n=============================");
		printf("\ntam. DISPONIBLES: %d",queue_size(DISPONIBLES));for(int i=0;i<queue_size(DISPONIBLES);i++){printf(" (%d)",get_id(list_get(DISPONIBLES->elements,i)));}
		//printf("\ntam. POKEMONS: %d",queue_size(POKEMONS));
		printf("\ntam. DEADLOCKS: %d",queue_size(DEADLOCKS));for(int i=0;i<queue_size(DEADLOCKS);i++){printf(" (%d)",get_id(list_get(DEADLOCKS->elements,i)));}
		printf("\ntam. PREPARADOS: %d",queue_size(PREPARADOS));for(int i=0;i<queue_size(PREPARADOS);i++){printf(" (%d)",get_id(list_get(PREPARADOS->elements,i)));}
		printf("\ntam. ESPERANDO: %d",queue_size(ESPERANDO));for(int i=0;i<queue_size(ESPERANDO);i++){printf(" (%d)",get_id(list_get(ESPERANDO->elements,i)));}
		printf("\ntam. EJECUTADOS: %d",queue_size(EJECUTADOS));for(int i=0;i<queue_size(EJECUTADOS);i++){printf(" (%d)",get_id(list_get(EJECUTADOS->elements,i)));}
		printf("\n=============================\n");
}

int get_id(Entrenador* persona){
	return persona->idEntrenador;
}

int getIndice(int indiceAEncontrar){
	int indice=0;
	int sigo=1;
	//pthread_mutex_lock(&modificar_cola_deadlocks);
		while(indice < queue_size(DEADLOCKS) && sigo){
			if(indiceAEncontrar == get_id(list_get(DEADLOCKS->elements,indice))){
				sigo=0;
			}else{
				indice++;
			}
		}
		//pthread_mutex_unlock(&modificar_cola_deadlocks);
	return indice;
}

int transformar_indice(Entrenador* entrenador,char* pokemonDar){
	int indiceReal = 0;
	int encontrado = 1;

	while((indiceReal<list_size(entrenador->mios)) && encontrado){
		if(strcmp(pokemonDar,retornarNombrePosta(list_get(entrenador->mios,indiceReal)))==0){
			encontrado=0;
		}else{
			indiceReal++;
		}
	}

	return indiceReal;
}

int a_e1_le_interesa_alguno_de_e2(Entrenador* e1, Entrenador* e2){
	int encontrado = 1;
	t_list* objetivos2 = list_map(e1->objetivosActuales,(void*)retornarNombreFantasia);
	t_list* pertenecientes1 = list_map(e2->pertenecientesIntercambiables,(void*)retornarNombrePosta);
	int x1 = 0, y1;

	while((x1<list_size(pertenecientes1))&&encontrado){
		y1 = 0;

		while(y1<list_size(objetivos2) && encontrado){
			if(strcmp(list_get(pertenecientes1,x1), list_get(objetivos2,y1))==0){
				encontrado = 0;	// E2 TIENE UN POKEMON QUE LE INTERESA A E1
				e2->indiceDar = x1;
			} else {
				y1++;
			}
		}

		if(encontrado){
			x1++;
		}
	}

	if(encontrado==0){ //KHÉEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE

		//RETURN !ENCONTRADO;
		//LA PUTA MADRE.


		//printf("\nhay interes de %d por %d",e1->idEntrenador,e2->idEntrenador);
		return 1;
	}else{
		//printf("\nno hay interes de %d por %d",e1->idEntrenador,e2->idEntrenador);
		return 0;
	}
}

void intercambio_forzoso(Entrenador* mover, Entrenador* quieto){
	mover->tipoAccion = INTERCAMBIAR;
	mover->intentar_atrapar = list_get(quieto->mios,quieto->indiceDar);
	quieto->intentar_atrapar = list_get(mover->mios,mover->indiceDar);
	mover->movimiento[0] = ((quieto->posicion[0])-(mover->posicion[0]));
	mover->movimiento[1] = ((quieto->posicion[1])-(mover->posicion[1]));
	pthread_mutex_lock(&modificar_cola_deadlocks);

	pthread_mutex_lock(&modificar_cola_preparados);
	queue_push(PREPARADOS,list_get(DEADLOCKS->elements,getIndice(mover->idEntrenador)));
	//queue_push(PREPARADOS,mover);
	pthread_mutex_unlock(&modificar_cola_preparados);
	log_info(logInit," = %d = PASA A LA COLA PREPARADOS",mover->idEntrenador);
	
	pthread_mutex_lock(&modificar_cola_esperando);
	queue_push(ESPERANDO,list_get(DEADLOCKS->elements,getIndice(quieto->idEntrenador)));
	//queue_push(PREPARADOS,quieto);
	pthread_mutex_unlock(&modificar_cola_esperando);
	log_info(logInit," = %d = PASA A LA COLA ESPERANDO",quieto->idEntrenador);

	list_remove(DEADLOCKS->elements,getIndice(quieto->idEntrenador));
	list_remove(DEADLOCKS->elements,getIndice(mover->idEntrenador));

	log_info(logInit," = %d = VA A INTERCAMBIAR CON %d",mover->idEntrenador,quieto->idEntrenador);

	//printf("\nSE SUMA 1 HAY_PREPARADOS");
	sem_post(&hayPreparados);

	pthread_mutex_unlock(&modificar_cola_deadlocks);
	//printf("\n ENTRENADOR %d VA A DAR A %s",mover->idEntrenador,retornarNombrePosta(list_get(mover->mios,mover->indiceDar)));
	//printf("\n ENTRENADOR %d VA A DAR A %s",quieto->idEntrenador,retornarNombrePosta(list_get(quieto->mios,quieto->indiceDar)));

}

void verificar_espera_circular(){
	int temporal;
	int sigo;
	//pthread_mutex_lock(&modificar_cola_deadlocks);
	int datos[3][queue_size(DEADLOCKS)];
	int columnas = 0;
	// FILA --> ME INTERESA ALGUN POKEMON DE ESTOS
	// COLUMNA --> A ALGUNO DE ESTOS LES INTERESA MI POKEMON
	//fprintf(logTP,"Se inicia el algoritmo de deteccion de Deadlock: ");
	for(temporal=0;temporal < queue_size(DEADLOCKS);temporal++){
		datos[0][temporal] = -1;
		datos[1][temporal] = -1;
		datos[2][temporal] = get_id(list_get(DEADLOCKS->elements,temporal));
	}
	for(int actual=0;actual<queue_size(DEADLOCKS);actual++){
		temporal=0;
		sigo=1;
		while(sigo && (temporal<queue_size(DEADLOCKS))){
			if(temporal != actual){
				if(a_e1_le_interesa_alguno_de_e2(list_get(DEADLOCKS->elements,actual),list_get(DEADLOCKS->elements,temporal))){
					sigo=0;
					datos[0][actual] = temporal;
					datos[1][temporal] = actual;
				}
			}
			temporal++;
		}
	}
	for(temporal=0;temporal < queue_size(DEADLOCKS);temporal++){
		if( datos[0][temporal] != -1 && datos[1][temporal] != -1 ){
			columnas++;
		}
	}
	if(columnas > 2){
		int esperaCircular[3][columnas];
		int contador = -1;
		int b;

		for(temporal=0;temporal < queue_size(DEADLOCKS);temporal++){
			b = temporal;
			sigo = 1;

			do{
				if(datos[0][b] != -1 && datos[1][b] != -1){
					b = datos[0][b];
				}else{
					sigo=0;
				}
			} while(sigo && b != temporal);

			if(sigo){
				contador++;
				esperaCircular[0][contador] = datos[2][temporal];
				esperaCircular[1][contador] = datos[0][temporal];
				esperaCircular[2][contador] = datos[1][temporal];
			}
		}

		if(contador >= 2){
			DEADLOCKS_RESUELTOS += 1;
			//printf("\nhay espera circular");
			log_info(logInit,"SE DETECTO ESPERA CIRCULAR");
			//fprintf(logTP,"se detecto una situacion de Deadlock\n");
			/*
			printf("\n-------------------------------------------\n");
			for(temporal=0;temporal < columnas;temporal++){//ESTE ES EL INDICE DE DEADLOCKS
				printf("ID: %d (%d) // ESPERA: %d // RETIENE: %d \n",temporal,esperaCircular[0][temporal],esperaCircular[1][temporal],esperaCircular[2][temporal]);
			}
			printf("\n-------------------------------------------\n");// ESPERA CIRCULAR CONFIRMADA Y VERIFICADA CON 3 O + ENTRENADORES INVOLUCRADOS
			*/
			//printf("\nprimero: %d (dl : %d)",esperaCircular[0][0],getIndice(esperaCircular[0][0]));
			//printf("\nsegundo: %d (dl : %d)",esperaCircular[0][(esperaCircular[2][0])],getIndice(esperaCircular[0][(esperaCircular[2][0])]));

			intercambio_forzoso(list_get(DEADLOCKS->elements,getIndice(esperaCircular[0][0])),list_get(DEADLOCKS->elements,getIndice(esperaCircular[0][(esperaCircular[2][0])])));
		}else{
			//printf("\nno hay espera circular 1111");
			log_info(logInit,"NO SE DETECTO ESPERA CIRCULAR(1)");
			//fprintf(logTP,"no se detecto una situacion de Deadlock\n");
		}
	}else if(columnas==2){
		if(datos[0][0]==datos[1][1] && datos[1][0]==datos[0][1]){
			//printf("\nhay espera circular");
			log_info(logInit,"SE DETECTO ESPERA CIRCULAR");
			//fprintf(logTP,"se detecto una situacion de Deadlock\n");
			intercambio_forzoso(list_get(DEADLOCKS->elements,0),list_get(DEADLOCKS->elements,1));
		}else{
			//printf("\nno hay espera circular 2222");
			log_info(logInit,"NO SE DETECTO ESPERA CIRCULAR(2)");
			//fprintf(logTP,"no se detecto una situacion de Deadlock\n");
		}
	}else{
		//printf("\nno hay espera circular 3333");
		log_info(logInit,"NO SE DETECTO ESPERA CIRCULAR(3)");
		//fprintf(logTP,"no se detecto una situacion de Deadlock\n");
	}
	//printf("\nSE FINALIZA LA VERIFICACION DE ESPERA CIRCULAR");
	log_info(logInit,"SE FINALIZA LA VERIFICACION DE ESPERA CIRCULAR");
	//pthread_mutex_unlock(&modificar_cola_deadlocks);
}

int verificar_intercambio(Entrenador* e1,Entrenador* e2){
	int hayIntercambio=0;

	t_list* pertenecientes1 = list_map(e1->pertenecientesIntercambiables,(void*) retornarNombrePosta);
	t_list* pertenecientes2 = list_map(e2->pertenecientesIntercambiables,(void*) retornarNombrePosta);
	t_list* objetivos1 = list_map(e1->objetivosActuales,(void*) retornarNombreFantasia);
	t_list* objetivos2 = list_map(e2->objetivosActuales,(void*) retornarNombreFantasia);

		int x1=0;
		int y1;
		int x2=0;
		int y2;
		int encontrado=1;

		while((x1<list_size(pertenecientes1))&&encontrado){
			y1=0;
			while(y1<list_size(objetivos2)&&encontrado){
				if(strcmp(list_get(pertenecientes1,x1),list_get(objetivos2,y1))==0){
					encontrado=0;	// E1 TIENE UN POKEMON QUE LE INTERESA A E2
				}else{
					y1++;
				}
			}
			if(encontrado){
				x1++;
			}
		}
		if(encontrado==0){
			encontrado=1;
			while((x2<list_size(pertenecientes2))&&encontrado){
				y2=0;
				while(y2<list_size(objetivos1)&&encontrado){
					if(strcmp(list_get(pertenecientes2,x2),list_get(objetivos1,y2))==0){
						encontrado=0;	// E2 TIENE UN POKEMON QUE LE INTERESA A E1
					}else{
						y2++;
					}
				}
				if(encontrado){
					x2++;
				}
			}
		}
		if(encontrado==0){// HAY INTERCAMBIO ENTRE ESTOS 2 ENTRENADORES
			x2 = transformar_indice(e2,list_get(pertenecientes2,x2));
			x1 = transformar_indice(e1,list_get(pertenecientes1,x1));

			e2->intentar_atrapar = list_get(e1->mios,x1);
			e1->intentar_atrapar = list_get(e2->mios,x2);
			e2->indiceDar = x2;
			e1->indiceDar = x1;
			e2->tipoAccion = INTERCAMBIAR;
			e2->movimiento[0] = ((e1->posicion[0])-(e2->posicion[0]));
			e2->movimiento[1] = ((e1->posicion[1])-(e2->posicion[1]));
			//printf("\n%d se va a mover X:%d // Y:%d",e2->idEntrenador,e2->movimiento[0],e2->movimiento[1]);
			hayIntercambio=1;
			//printf("\nhay intercambio entre %d y %d\n",e1->idEntrenador,e2->idEntrenador);
		}else{
			//printf("\nno hay intercambio entre %d y %d\n",e1->idEntrenador,e2->idEntrenador);
		}
	return hayIntercambio;
}

void verificar_intercambios(Entrenador* recienIngresado){
	//printf("\ntam DEADLOCKS: %d",queue_size(DEADLOCKS));
	//mostrarEntrenador(recienIngresado);
	int indice=0;
	int sigo=1;
	while((sigo==1) && (indice<(queue_size(DEADLOCKS)-1))){
		if(verificar_intercambio(list_get(DEADLOCKS->elements,indice),recienIngresado)){
			sigo=0;
			pthread_mutex_lock(&modificar_cola_deadlocks);
			list_remove(DEADLOCKS->elements,(list_size(DEADLOCKS->elements)-1));

			pthread_mutex_lock(&modificar_cola_preparados);
			queue_push(PREPARADOS,recienIngresado);
			pthread_mutex_unlock(&modificar_cola_preparados);
			log_info(logInit," = %d = PASA A COLA PREPARADOS",recienIngresado->idEntrenador);

			//fprintf(logTP,"Entrenador %d pasa de la cola DEADLOCKS a PREPARADOS: va a ir a la posicion de otro entrenador para realizar un intercambio\n",recienIngresado->idEntrenador);

			pthread_mutex_lock(&modificar_cola_esperando);
			queue_push(ESPERANDO,list_get(DEADLOCKS->elements,indice));
			pthread_mutex_unlock(&modificar_cola_esperando);
			log_info(logInit," = %d = PASA A COLA ESPERANDO",get_id(list_get(DEADLOCKS->elements,indice)));

			//fprintf(logTP,"Entrenador %d pasa de la cola DEADLOCKS a ESPERANDO: va a esperar a que otro entrenador vaya a su posicion para realizar un intercambio\n",get_id(list_get(DEADLOCKS->elements,indice)));

			list_remove(DEADLOCKS->elements,indice);
			sem_post(&hayPreparados);
			pthread_mutex_unlock(&modificar_cola_deadlocks);
		}else{
			indice++;
		}
	}

	if(sigo==1 && queue_size(DEADLOCKS)>2){ // QUIERE DECIR QUE NO VA A INTERCAMBIAR CON NINGUN OTRO EN DEADLOCK, ENTONCES VAMOS A VER SI HAY ESPERA CIRCULAR
		//printf("\nvamos a verificar si hay espera circular:");
		log_info(logInit,"SE INICIA ALGORITMO DE DETECCION DE ESPERA CIRCULAR");
		verificar_espera_circular();
	}

}

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

void intercambiar(Entrenador* persona, Entrenador* quieto){
	//fprintf(logTP,"Entrenador %d va a intercambiar con el Entrenador %d en (%d,%d)\n",persona->idEntrenador,quieto->idEntrenador,quieto->posicion[0],quieto->posicion[1]);

	list_add(persona->mios,persona->intentar_atrapar);
	list_add(quieto->mios,quieto->intentar_atrapar);

	list_remove(persona->mios,persona->indiceDar);
	list_remove(quieto->mios,quieto->indiceDar);

	int i=0;
	int encontrado=0;
	while((i<list_size(persona->objetivosActuales))&&(encontrado==0)){
		if(strcmp(retornarNombreFantasia(list_get(persona->objetivosActuales,i)),retornarNombrePosta(persona->intentar_atrapar))==0){
			encontrado=1;
			list_remove(persona->objetivosActuales,i);
		}
		i++;
	}
	i=0;
	encontrado=0;
	while((i<list_size(quieto->objetivosActuales))&&(encontrado==0)){
		if(strcmp(retornarNombreFantasia(list_get(quieto->objetivosActuales,i)),retornarNombrePosta(quieto->intentar_atrapar))==0){
			encontrado=1;
			list_remove(quieto->objetivosActuales,i);
		}
		i++;
	}
	asignarPertenecientesIntercambiables(persona);
	asignarPertenecientesIntercambiables(quieto);

	quieto->intentar_atrapar = NULL;
	persona->intentar_atrapar = NULL;
	//printf("\nLUEGO DE HACER EL INTERCAMBIO QUEDAN ASI:\n");
	//mostrarEntrenador(quieto);
	//mostrarEntrenador(persona);
	/*
	pthread_mutex_lock(&modificar_cola_ejecutados);
	queue_push(EJECUTADOS,quieto); 
	pthread_mutex_unlock(&modificar_cola_ejecutados);
	*/
	pthread_mutex_lock(&modificar_cola_esperando);
	queue_pop(ESPERANDO);
	pthread_mutex_unlock(&modificar_cola_esperando);

	if(verificar_deadlock(quieto)){
		//fprintf(logTP,"Entrenador %d pasa de la cola ESPERANDO a DEADLOCKS: todavia no cumplio su objetivo\n",quieto->idEntrenador);
	}
	ingreso_a_colas_entrenador(quieto);
}

void atrapar(Entrenador* persona){
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
	if(encontrado==0){
		list_add(persona->pertenecientesIntercambiables,persona->intentar_atrapar);
		log_info(logInit,"= %d = ATRAPE A %s",persona->idEntrenador,(persona->intentar_atrapar)->nombre);
	}
	persona->intentar_atrapar = NULL;
}

void moverse(Entrenador* persona){
	//printf("\n[Entrenador %d] X INICIAL: %d, Y INICIAL: %d",persona->idEntrenador,persona->posicion[0],persona->posicion[1]);
	if(strcmp(algoritmo,"FIFO")==0){
		while(persona->movimiento[0]!=0 || persona->movimiento[1]!=0){
			if(persona->movimiento[0]==0){}else{
				if(persona->movimiento[0] < 0){
					persona->movimiento[0] += 1;
					persona->posicion[0] -= 1;
					sumarQuantum(persona->idEntrenador,1);
					sleep(tiempoRetardo);
				}else{
					persona->movimiento[0] -= 1;
					persona->posicion[0] += 1;
					sumarQuantum(persona->idEntrenador,1);
					sleep(tiempoRetardo);
				}
			}
			if(persona->movimiento[1]==0){}else{
				if(persona->movimiento[1] < 0){
					persona->movimiento[1] += 1;
					persona->posicion[1] -= 1;
					sumarQuantum(persona->idEntrenador,1);
					sleep(tiempoRetardo);
				}else{
					persona->movimiento[1] -= 1;
					persona->posicion[1] += 1;
					sumarQuantum(persona->idEntrenador,1);
					sleep(tiempoRetardo);
				}
			}
		}
	}else{ // RR
		while((persona->movimiento[0]!=0 || persona->movimiento[1]!=0)&&(persona->quantum_restante>0)){
					if(persona->movimiento[0]==0){}else{
						if(persona->movimiento[0] < 0){
							persona->movimiento[0] += 1;
							persona->posicion[0] -= 1;
							persona->quantum_restante--;
							sumarQuantum(persona->idEntrenador,1);
							sleep(tiempoRetardo);
						}else{
							persona->movimiento[0] -= 1;
							persona->posicion[0] += 1;
							persona->quantum_restante--;
							sumarQuantum(persona->idEntrenador,1);
							sleep(tiempoRetardo);
						}
					}
					
					if(persona->quantum_restante>0){
						if(persona->movimiento[1]==0){}else{
							if(persona->movimiento[1] < 0){
								persona->movimiento[1] += 1;
								persona->posicion[1] -= 1;
								persona->quantum_restante--;
								sumarQuantum(persona->idEntrenador,1);
								sleep(tiempoRetardo);
							}else{
								persona->movimiento[1] -= 1;
								persona->posicion[1] += 1;
								persona->quantum_restante--;
								sumarQuantum(persona->idEntrenador,1);
								sleep(tiempoRetardo);
							}
						}
					}
		}
	}
	//fprintf(logTP,"Entrenador %d se movio a (%d,%d)\n",persona->idEntrenador,persona->posicion[0],persona->posicion[1]);
	//printf("\n[Entrenador %d] X FINAL: %d, Y FINAL: %d",persona->idEntrenador,persona->posicion[0],persona->posicion[1]);
}

void ingreso_a_colas_entrenador(Entrenador* persona){
	actualizar_estado(persona);
	switch(persona->estado){
		case NUEVO:
			pthread_mutex_lock(&modificar_cola_disponibles);
			queue_push(DISPONIBLES,persona);
			pthread_mutex_unlock(&modificar_cola_disponibles);
			log_info(logInit," = %d = PASA A LA COLA DISPONIBLES",persona->idEntrenador);
			if(queue_size(POKEMONS)>0){
				entrenador_mas_cercano(queue_peek(POKEMONS));
			}
			break;
		case BLOQUEADO:
			if(verificar_deadlock(persona) == 0){
				pthread_mutex_lock(&modificar_cola_disponibles);
				queue_push(DISPONIBLES,persona);
				pthread_mutex_unlock(&modificar_cola_disponibles);
				log_info(logInit," = %d = PASA A LA COLA DISPONIBLES",persona->idEntrenador);
				if(queue_size(POKEMONS)>0){
					entrenador_mas_cercano(queue_peek(POKEMONS));
				}
				//fprintf(logTP,"Entrenador %d pasa de la cola EJECUTADOS a DISPONIBLES: tiene espacio para atrapar otro pokemon\n",persona->idEntrenador);
			}
			else{
				pthread_mutex_lock(&modificar_cola_deadlocks);
				queue_push(DEADLOCKS,persona);
				pthread_mutex_unlock(&modificar_cola_deadlocks);
				log_info(logInit," = %d = PASA A LA COLA DEADLOCKS",persona->idEntrenador);
				if(queue_size(DEADLOCKS)>1){
					verificar_intercambios(persona);
				}
			}
			break;
		default:
			log_info(logInit,"= %d = FINALIZA SU MISION",persona->idEntrenador);
	}
}

void reintegrar_quantum(Entrenador* entrenador){
	printf("\nSIN QUANTUM");
	pthread_mutex_lock(&modificar_cola_preparados);
	queue_push(PREPARADOS,entrenador);
	pthread_mutex_unlock(&modificar_cola_preparados);
	entrenador->quantum_restante += configTeam->quantum;
	//fprintf(logTP,"Entrenador %d pasa de la cola PREPARADOS a PREPARADOS: se le acabo el quantum\n",entrenador->idEntrenador);
	sem_post(&hayPreparados);
}

void ubicar_entrenador(Entrenador* entrenador){
	if(verificar_deadlock(entrenador)==0){
		pthread_mutex_lock(&modificar_cola_ejecutados);
		queue_push(EJECUTADOS,(entrenador));
		pthread_mutex_unlock(&modificar_cola_ejecutados);
		log_info(logInit," = %d = PASA A LA COLA EJECUTADOS",entrenador->idEntrenador);
		//fprintf(logTP,"Entrenador %d pasa de la cola PREPARADOS a EJECUTADOS: tiene espacio para atrapar otro pokemon, o se va a ir del sistema",(entrenador)->idEntrenador);
	}else{
		pthread_mutex_lock(&modificar_cola_deadlocks);
		queue_push(DEADLOCKS,entrenador);
		pthread_mutex_unlock(&modificar_cola_deadlocks);
		log_info(logInit," = %d = PASA A LA COLA DEADLOCKS",entrenador->idEntrenador);
		//fprintf(logTP,"Entrenador %d pasa de la cola PREPARADOS a DEADLOCKS: no tiene espacio para atrapar otro pokemon, y todavia no cumplió su objetivo\n",(entrenador)->idEntrenador);
		if(queue_size(DEADLOCKS)>1){
			verificar_intercambios(entrenador);
		}
	}
}

int suma(int x, int y){
	if(x<0){x = x * (-1);}
	if(y<0){y = y * (-1);}
	return (x+y);
}

void iniciar_entrenador(Entrenador** entrenador){
	while(verificar_finalizacion(*entrenador)==0){
		log_info(logInit,"= %d = BLOQUEADO",(*entrenador)->idEntrenador);
		sem_wait(&((*entrenador)->activador));
		log_info(logInit,"= %d = SE VA A MOVER %d",(*entrenador)->idEntrenador,suma((*entrenador)->movimiento[0],(*entrenador)->movimiento[1]));
		moverse(*entrenador);
		if((*entrenador)->movimiento[0]==0 && (*entrenador)->movimiento[1]==0){
			log_info(logInit,"= %d = LLEGUE A MI OBJETIVO",(*entrenador)->idEntrenador);// LLEGO A SU OBJETIVO
			if((*entrenador)->tipoAccion==ATRAPAR){
				if(strcmp(algoritmo,"RR")==0){ // RR
					if((*entrenador)->quantum_restante >= 1){ // TIENE QUANTUM COMO PARA HACER CONSULTA A BROKER
						//fprintf(logTP,"Entrenador %d va a intentar atrapar a %s en (%d,%d)\n",(*entrenador)->idEntrenador,retornarNombrePosta((*entrenador)->intentar_atrapar),((*entrenador)->intentar_atrapar)->x,((*entrenador)->intentar_atrapar)->y);
						
						// AGREGAR FPRINTF PARA EL CASO DE QUE HAYA O NO ATRAPADO AL POKEMON

						/*

						MENSAJES CATCH - CAUGHT, depende del mensaje de Broker si entrenador atrapar o nó al pokemon

						*/

						atrapar(*entrenador);

						((*entrenador)->quantum_restante)--;
						sumarQuantum((*entrenador)->idEntrenador,1);

						//sleep(tiempoRetardo);
						if((*entrenador)->quantum_restante == 0){
							(*entrenador)->quantum_restante += configTeam->quantum;
						}
						ubicar_entrenador(*entrenador);
					}else{
						reintegrar_quantum(*entrenador);
					}
				}else{ // FIFO
					//fprintf(logTP,"Entrenador %d va a intentar atrapar a %s en (%d,%d)\n",(*entrenador)->idEntrenador,retornarNombrePosta((*entrenador)->intentar_atrapar),((*entrenador)->intentar_atrapar)->x,((*entrenador)->intentar_atrapar)->y);
					// AGREGAR FPRINTF PARA EL CASO DE QUE HAYA O NO ATRAPADO AL POKEMON
					atrapar(*entrenador);
					sumarQuantum((*entrenador)->idEntrenador,1);
					ubicar_entrenador(*entrenador);
				}
			}else{ // INTERCAMBIO
				if(strcmp(algoritmo,"RR")==0){ // RR
					if((*entrenador)->quantum_restante >= 5){ // TIENE QUANTUM COMO PARA HACER UN INTERCAMBIO
						intercambiar(*entrenador,queue_peek(ESPERANDO));

						(*entrenador)->quantum_restante -= 5;
						sumarQuantum((*entrenador)->idEntrenador,5);

						if((*entrenador)->quantum_restante == 0){
							(*entrenador)->quantum_restante += configTeam->quantum;
						}
						ubicar_entrenador(*entrenador);
					}else{
						reintegrar_quantum(*entrenador);
					}
				}else{ // FIFO
					intercambiar(*entrenador,queue_peek(ESPERANDO));
					sumarQuantum((*entrenador)->idEntrenador,5);
					ubicar_entrenador(*entrenador);
				}
			}
		}else{ // EL QUANTUM LE QUEDO CORTO
			reintegrar_quantum(*entrenador);
		}
		log_info(logInit,"= %d = YA TERMINE DE EJECUTARME",(*entrenador)->idEntrenador);
		//mostrarEntrenador(*entrenador);
		sem_post(&ya_termine);
	}
	log_info(logInit,"pthread_exit(%d)",(*entrenador)->idEntrenador);
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

int cantidad_arrays(char** array){ // retorna la cantidad posta de un array
		int indice=0;
		while(array[indice] !=NULL){
				indice++;
		}
		return indice;
}

void cargarConfig(Config* conexionConfig, t_log* logger){
	t_config* config;
	config = leer_config("configTeam.config", logger);
	conexionConfig->posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES"); // lista de strings, ultimo elemento nulo
	conexionConfig->pertenecientes = config_get_array_value(config,"POKEMON_ENTRENADORES");
	conexionConfig->objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
	conexionConfig->ip = config_get_string_value(config, "IP_BROKER");
	conexionConfig->puerto = config_get_string_value(config, "PUERTO_BROKER");
	conexionConfig->ipTeam = config_get_string_value(config, "IP_TEAM");
	conexionConfig->puertoTeam = config_get_string_value(config, "PUERTO_TEAM");
	conexionConfig->tiempoReconexion = config_get_int_value(config, "TIEMPO_RECONEXION");
	conexionConfig->ID = 4 + config_get_int_value(config, "ID"); //Cosas turbias del Broker.
	algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	tiempoRetardo = config_get_int_value(config, "RETARDO_CICLO_CPU");

	printf("ALGORITMO DE PLANIFICACION = %s\n",algoritmo);
	CANT_ENTRENADORES = cantidad_arrays(conexionConfig->pertenecientes);
	DEADLOCKS_RESUELTOS=0;
	CAMBIOS_CONTEXTO=0;
	/*
	if(strcmp(algoritmo,"RR")==0){
		configTeam->quantum = config_get_int_value(config,"QUANTUM");
	}
	*/
	METRICAS_ENTRENADORES = list_create();
	for(int i=0; i < CANT_ENTRENADORES;i++){
		Metrica *unaMetrica = malloc(sizeof(Metrica));
		unaMetrica->idEntrenador = i;
		unaMetrica->quantumTotal = 0;
		list_add(METRICAS_ENTRENADORES,unaMetrica);
	}
	logTP  = fopen (conexionConfig->path, "w");	
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
	if(pokemons[0]!=' '){
		char* token = strtok(pokemons, "|");
		persona->mios = list_create();
		while (token != NULL) {
			Pokemon* p;
			p = crearPokemon(token,0,0);
			list_add(persona->mios,p);
			token = strtok(NULL, "|");
		}
	}
}
void asignarObjetivos(Entrenador* persona,char* pokemons){
	if(pokemons[0]!=' '){
		char* token = strtok(pokemons, "|");
			persona->objetivos = list_create();
			while (token != NULL) {
				PokemonFantasia* p;
				p = crearObjetivo(token);
				list_add(persona->objetivos,p);
				token = strtok(NULL, "|");
			}
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

void mostrarPertenecientesIntercambiables(Entrenador* entrenador){
	t_list* listaObjetivos = entrenador->pertenecientesIntercambiables;
	printf("\nPokemones para intercambiar: ");
	for(int i = 0 ; i<list_size(listaObjetivos);i++){
		Pokemon* pokemon = list_get(listaObjetivos,i);
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
	mostrarPertenecientesIntercambiables(entrenador);
}

char* retornarNombrePosta(Pokemon* p){
	return p->nombre;
}
char* retornarNombreFantasia(PokemonFantasia* p){
	return p->nombre;
}
int retornarPosicion(Pokemon* p,int eje){
	if(eje==0){
		return p->x;
	}else{
		return p->y;
	}
}

void asignarPertenecientesIntercambiables(Entrenador* persona){
	persona->pertenecientesIntercambiables = list_create();
	t_list* nombresObjetivos = list_map(persona->objetivos,(void*) retornarNombreFantasia);
	t_list* nombresPertenecientes = list_map(persona->mios,(void*) retornarNombrePosta);
	int indicePertenecientes=0;
	int indiceObjetivos;
	int repetido;
	int noRepetido;
	if(list_size(nombresPertenecientes)>0){
		while(indicePertenecientes<list_size(nombresPertenecientes)){
			indiceObjetivos=0;
			repetido=1;
			noRepetido=0;
			while((indiceObjetivos<list_size(nombresObjetivos))&&(repetido)){
				if(!strcmp(list_get(nombresPertenecientes,indicePertenecientes),list_get(nombresObjetivos,indiceObjetivos))){
					repetido=0;
					list_remove(nombresObjetivos,indiceObjetivos);
					list_remove(nombresPertenecientes,indicePertenecientes);
					indicePertenecientes--;
				}else{
					noRepetido++;
				}
				indiceObjetivos++;
			}
			indicePertenecientes++;
		}
		if(list_size(nombresPertenecientes)>0){
			for(int i=0;i<list_size(nombresPertenecientes);i++){
				Pokemon* p;
				p = crearPokemon(list_get(nombresPertenecientes,i),0,0);
				list_add(persona->pertenecientesIntercambiables,p);
			}
		}
	}else{printf("\n%d no tiene pokemons",persona->idEntrenador);}
}

Entrenador* inicializarEntrenador(int id,char* posicion, char* pokePertenecientes , char* pokeObjetivos){
	Entrenador* entrenador = (Entrenador*)malloc(sizeof(Entrenador));
	entrenador->idEntrenador = id;
	asignarPosicion(entrenador,posicion);
	asignarPertenecientes(entrenador,pokePertenecientes);
	asignarObjetivos(entrenador,pokeObjetivos);
	entrenador->estado = NUEVO;
	asignarObjetivosActuales(entrenador);
	asignarPertenecientesIntercambiables(entrenador);

	

	if(verificar_deadlock(entrenador)==1){
		entrenador->estado = BLOQUEADO;
	}
	if(verificar_finalizacion(entrenador)==0){
		entrenador->movimiento[0]=0;entrenador->movimiento[1]=0;
		entrenador->intentar_atrapar=NULL;
		sem_init(&(entrenador->activador),0,0);/*
		if(strcmp(algoritmo,"RR")==0){
			entrenador->quantum_restante = configTeam->quantum;
		}*/
	}else{
		entrenador->estado = SALIR;
	}
	mostrarEntrenador(entrenador);
	return entrenador;

}

void crearObjetivoGlobal(char* especiePokemon){
	Objetivo *new = malloc(sizeof(Objetivo));
    new->especie = strdup(especiePokemon);
    new->cantidad = 1;
    new->aceptarMas = 1;
    list_add(OBJETIVO_GLOBAL,new);
}

Objetivo* getObj(Objetivo* x){
	return x;
}

int yaEsObjetivoGlobal(PokemonFantasia* especiePokemon){
	int loEs=0;

	for(int i=0;i < list_size(OBJETIVO_GLOBAL);i++){
		if(strcmp(getObj(list_get(OBJETIVO_GLOBAL,i))->especie,especiePokemon->nombre)==0){
			loEs=1;
			getObj(list_get(OBJETIVO_GLOBAL,i))->cantidad += 1;
		}
	}
	return loEs;
}
/*
void eliminarObjetivosGlobales(Objetivo* x){
	free(x->cantidad);
	free(x->especie);
	free(x);
}
*/
void getObjetivosGlobales(Team team){/*
	if(list_size(OBJETIVO_GLOBAL)>0){
		list_clean_and_destroy_elements(OBJETIVO_GLOBAL,(void*)eliminarObjetivosGlobales);
	}*/

	for(int i = 0 ;i<CANT_ENTRENADORES;i++){
		for(int j = 0 ; j<list_size(team[i]->objetivosActuales);j++){
			if(yaEsObjetivoGlobal(list_get(team[i]->objetivosActuales,j))){
				// no hace nada
			}else{crearObjetivoGlobal(retornarNombreFantasia(list_get(team[i]->objetivosActuales,j)));}
			//list_add(OBJETIVO_GLOBAL,list_get(team[i]->objetivosActuales,j));
		}
	}
}

Team inicializarTeam(char** posiciones, char** pokePertenecientes , char** pokeObjetivos){
	Entrenador** team = (Entrenador**)(malloc(sizeof(Entrenador)));
	logInit = iniciar_logger("teamInit.log","Init");
	sem_init(&esperar_pokemons,0,0);
	sem_init(&s_match,0,0);
	sem_init(&s_ejecucion,0,0);
	sem_init(&mas_pokemons,0,1);
	sem_init(&ya_termine,0,0);
	sem_init(&siguiente,0,0);
	sem_init(&rellenar_colas,0,1);
	sem_init(&esperar_finalizacion,0,0);
	sem_init(&finalizar_ejecucion,0,0);
	sem_init(&intercambio_hecho,0,0);
	sem_init(&hayPreparados,0,0);

	OBJETIVO_GLOBAL = list_create();
	//OBJETIVO_GLOBAL_FILTRADO = list_create();
	
	PREPARADOS = queue_create();
	DISPONIBLES = queue_create();
	DEADLOCKS = queue_create();
	POKEMONS = queue_create();
	EJECUTADOS = queue_create();
	ESPERANDO = queue_create();
	pthread_mutex_init(&modificar_cola_preparados,NULL);
	pthread_mutex_init(&modificar_cola_disponibles,NULL);
	pthread_mutex_init(&modificar_cola_deadlocks,NULL);
	pthread_mutex_init(&modificar_cola_pokemons,NULL);
	pthread_mutex_init(&modificar_cola_ejecutados,NULL);
	pthread_mutex_init(&modificar_cola_esperando,NULL);
	sem_init(&progreso,0,1);
	pthread_t hilo[cantidad_arrays(posiciones)];
	for(int i=0 ; i<cantidad_arrays(posiciones) ; i++){
		team[i] = inicializarEntrenador(i,posiciones[i],pokePertenecientes[i],pokeObjetivos[i]);
		ingreso_a_colas_entrenador(team[i]);
		pthread_create(&hilo[i],NULL,(void*)iniciar_entrenador,&(team[i]));
		pthread_detach(hilo[i]);
	}
	//getObjetivosGlobales(team);
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

void destructorPokemonsFantasia(PokemonFantasia* pokemon){
	free(pokemon->nombre);
	free(pokemon);
 }

void liberarTeam(Team team){
	/*for(int i=0 ; i< CANT_ENTRENADORES;i++){
		free(team[i]);
	}*/
	sem_destroy(&esperar_pokemons);
	sem_destroy(&s_match);
	sem_destroy(&s_ejecucion);
	sem_destroy(&mas_pokemons);
	sem_destroy(&ya_termine);
	sem_destroy(&siguiente);
	sem_destroy(&rellenar_colas);
	sem_destroy(&esperar_finalizacion);
	sem_destroy(&finalizar_ejecucion);
	sem_destroy(&intercambio_hecho);
	sem_destroy(&hayPreparados);

	//list_destroy_and_destroy_elements( OBJETIVO_GLOBAL ,(void*) destructorPokemonsFantasia);

	queue_destroy(PREPARADOS);
	queue_destroy(DISPONIBLES);
	queue_destroy(DEADLOCKS);
	queue_destroy(POKEMONS);
	queue_destroy(EJECUTADOS);
	queue_destroy(ESPERANDO);

	//free(team);
}
