#include "planificacion.h"

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
	int indice_menor=0;
	float masCerca = distancia(team[0]->posicion[0],team[0]->posicion[1],p_x,p_y);
	for(int i=0;i<(sizeof(team)-1);i++){
		printf("distancia del entrenador %d = %.2f \n",team[i]->idEntrenador,distancia(team[i]->posicion[0],team[i]->posicion[1],p_x,p_y));
		if(masCerca > distancia(team[i]->posicion[0],team[i]->posicion[1],p_x,p_y)){
			masCerca = distancia(team[i]->posicion[0],team[i]->posicion[1],p_x,p_y);
			indice_menor = i;
		}
	}
	printf("%d es el mas cercano %.2f",indice_menor,masCerca);
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
	/* V1.0 de fifo, llega un pokemon nuevo, con los objetivos globales verificar si el pokemon es
	 * requerido en el team, si lo es; se calcula el entrenador mas cercano, debe moverse (gabo: el
	 * entrenador se mueve dentro de su hilo)
	 */
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
