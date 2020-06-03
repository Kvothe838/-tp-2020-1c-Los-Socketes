#include "planificacion.h"

void asignar_movimiento(Entrenador* entrenador,int mov_x,int mov_y){
	entrenador->movimiento[0]=mov_x;
	entrenador->movimiento[1]=mov_y;
	printf("%d se mueve %d en el eje X, %d en el eje Y \n",entrenador->idEntrenador,entrenador->movimiento[0],entrenador->movimiento[1]);
	sem_post(&(entrenador->activador));
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

void cercania(Team team,int p_x, int p_y){ // se manda el team, y la posicion X e Y del pokemon recien aparecido
	int indice_menor=0;int mov_x;int mov_y;
	float masCerca = distancia(team[0]->posicion[0],team[0]->posicion[1],p_x,p_y);
	mov_x=(p_x - team[0]->posicion[0]);
	mov_y=(p_y - team[0]->posicion[1]);

	for(int i=0;i<(sizeof(team)-1);i++){
		printf("distancia del entrenador %d = %.2f \n",team[i]->idEntrenador,distancia(team[i]->posicion[0],team[i]->posicion[1],p_x,p_y));
		if(masCerca > distancia(team[i]->posicion[0],team[i]->posicion[1],p_x,p_y)){
			masCerca = distancia(team[i]->posicion[0],team[i]->posicion[1],p_x,p_y);
			indice_menor = i;
			mov_x=(p_x - team[i]->posicion[0]);
			mov_y=(p_y - team[i]->posicion[1]);
		}
	}
	printf("%d es el mas cercano %.2f \n",indice_menor,masCerca);
	asignar_movimiento(team[indice_menor],mov_x,mov_y);
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

void planificacion_fifo(Team team){
	char pokemon[10];int x;int y;
	printf("\n");
	printf("\n cant objetivos globales:%d \n",list_size(OBJETIVO_GLOBAL));
	for(int x=0;x<list_size(OBJETIVO_GLOBAL);x++){
		printf("%s \n",retornarNombreFantasia(list_get(OBJETIVO_GLOBAL,x)));
	}

	printf("\n Datos pokemon nuevo: ");
	scanf( "%s", pokemon );
	scanf("%d",&x);
	scanf("%d",&y);
	printf("\n pokemon aparecido: %s // %d // %d",pokemon,x,y);
	while(strcmp(pokemon,"nada")!=0){
		if(loNecesito(pokemon)==1){
			cercania(team,x,y);
		}else{printf("\n no necesito a este pokemon.");}

		printf("\n Datos pokemon nuevo: ");
		scanf( "%s", pokemon );
		scanf("%d",&x);
		scanf("%d",&y);
		printf("\n pokemon aparecido: %s // %d // %d",pokemon,x,y);
	}
}
