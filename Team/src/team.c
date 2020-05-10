#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#define MAX 2
#define MAX_CANT_OBJETIVOS 10


typedef struct entrenador{
	uint32_t ID_entrenador;
	uint32_t posicion[2];
	char pokemons_pertenecientes[MAX_CANT_OBJETIVOS];
	char pokemons_objetivos[MAX_CANT_OBJETIVOS];
	// ESTADO --> BLOQUEADO, EXEC, NEW, ETC...
}Entrenador;

int cantidad_comas(char*lista_posiciones){
	int comas = 0;
	for(int i=0;i<strlen(lista_posiciones);i++){
		if(lista_posiciones[i]==','){
			comas++;
		}
	}
	return comas;
}

int posicionar_entrenador(struct entrenador *entrenador, char* lista_posiciones, int indice_actual){
	int mismo_entrenador=1;
	char* temporal;
	int indice_temporal=0;
	int eje=0;
	while(mismo_entrenador){
		if(lista_posiciones[indice_actual]!=',' && lista_posiciones[indice_actual]!='\0' && lista_posiciones[indice_actual]!=']'){
			if(lista_posiciones[indice_actual]!='|'){
				// ACA HAY CARACTER VALIDO
				temporal[indice_temporal] = lista_posiciones[indice_actual];
				indice_temporal++;
				indice_actual++;
			}else{
				// ACA HAY UNA BARRA
				// ASIGNAR VALOR A POSICION
				temporal[indice_temporal] = '\0';
				entrenador->posicion[eje]= atoi(temporal);
				printf("%u|",entrenador->posicion[eje]);
				indice_temporal=0;
				indice_actual++;
				eje++;
			}
		}else{
			// ACA HAY CARACTER , ]
			temporal[indice_temporal] = '\0';
			entrenador->posicion[eje]= atoi(temporal);
			printf("%u \n",entrenador->posicion[eje]);
			mismo_entrenador=0;
		}
	}
	return (indice_actual+1);
}

int main(void) {
	char* lista_posiciones;
	char* lista_pokemons;
	char* lista_objetivos;
	t_log* logger;
	t_config* config;
	logger = iniciar_logger("Team.log", "Team");
	config = leer_config("configTeam.config", logger);
	lista_posiciones = config_get_string_value(config, "POSICIONES_ENTRENADORES");
	lista_pokemons = config_get_string_value(config, "POKEMON_ENTRENADORES");
	lista_objetivos = config_get_string_value(config, "OBJETIVOS_ENTRENADORES");

	int cantidad_entrenadores = cantidad_comas(lista_posiciones) +1;
	printf("cantidad entrenadores en el archivo= %d \n",cantidad_entrenadores);

	struct entrenador entrenador[cantidad_entrenadores];
	//pthread_t hilo[cantidad_entrenadores];
	int indice = 1;

	for(int x=0;x<cantidad_entrenadores;x++){
		printf("inicia entrenador \n");
		indice = posicionar_entrenador(&(entrenador[x]),lista_posiciones,indice);
		printf("fin entrenador \n");
	}

		//pthread_create(&hilo[b], NULL, &inicializar_entrenador, NULL);










/*
	for(int c=0;c<cantidad_entrenadores;c++){
		printf("se murio un hilo \n");
		pthread_join(&hilo[c],NULL);
	}
*/
	printf(" \n programa finalizado");
}
