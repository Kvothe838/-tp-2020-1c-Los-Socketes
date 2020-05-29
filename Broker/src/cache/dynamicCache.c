/*
 * dynamicCache.c
 *
 *  Created on: 25 may. 2020
 *      Author: utnso
 */


#include "dynamicCache.h"

int getBestFit(t_dynamic_table_entry tablaVacios[], int sizeTable, int desiredSize) {
	int i = 0;
	int found = -1;
	int bestPosition, bestDifference = 9999999;
	for(i=0; i<sizeTable; i++){
		t_dynamic_table_entry currentEntry = tablaVacios[i];
		if(!currentEntry.isEmpty && currentEntry.size >= desiredSize){
			int diff = currentEntry.size - desiredSize;
			if(diff < bestDifference){
				bestDifference = diff;
				bestPosition = i;
				found = 1;
			}
		}
	}
	return found >= 0 ? bestPosition : found;
}

int hayEspacio(t_dynamic_table_entry tablaVacios[], int tamanioTabla, int espacioRequerido, char* algoritmo, int *posicion){
	int posEspacioMinimo = 0;
	if(strcmp(algoritmo, "FF") == 0){
		for(*posicion = 0; *posicion < tamanioTabla; (*posicion)++){
			if(!tablaVacios[*posicion].isEmpty && tablaVacios[*posicion].size >= espacioRequerido)
				return 1;
		}

	}
	else {
		int bestPosition = getBestFit(tablaVacios, tamanioTabla, espacioRequerido);
		if(bestPosition >= 0){
			*posicion = bestPosition;
			return 1;
		}
		return 0;
	}

	return 0;
}

int obtenerParticion(t_dynamic_table_entry table[], int tamanioTabla, int esVacio){
	for(int i = 0; i < tamanioTabla; i++){
		if(table[i].isEmpty == esVacio)
			return i;
	}
	return -1;
}

void modificarTablaVacio(t_dynamic_table_entry table[], int espacioRequerido, int posicionVector, int tamanioParticionMinima){
	int desplazamiento = obtenerDesplazamientoMinimo(tamanioParticionMinima, espacioRequerido);
	if((table[posicionVector].size -= desplazamiento) == 0){
		table[posicionVector].position = 0;
		table[posicionVector].id = -1;
		table[posicionVector].isEmpty = 1;
	}
	else{
		table[posicionVector].position += desplazamiento;
	}

}

int obtenerDesplazamientoMinimo(int tamanioParticionMinima, int espacioRequerido){
	int posicionRelativa = 0;
	while((posicionRelativa += tamanioParticionMinima) < espacioRequerido);
	return posicionRelativa;
}

int obtenerTablaPorId(int id, int tamanio, t_dynamic_table_entry tablaElementos[]){
	for(int i = 0; i < tamanio; i++){
		if(tablaElementos[i].id == id)
			return i;
	}
	return -1;
}

void initializeTable(int tamanioCache, int tableSize, t_dynamic_table_entry table[], int esVacio){
	if(esVacio){
		table[0].id = 0;
		table[0].position= 0;
		table[0].size = tamanioCache;
		table[0].isEmpty = 0;
	}
	for(int i = esVacio; i < tableSize; i++){
		table[i].id = -1;
		table[i].position= 0;
		table[i].size = 0;
		table[i].isEmpty = 1;
	}
}

void agregarItem(	void* item, int tamanioItem, int tamanioTabla, int tamanioParticionMinima,
					char* algoritmo,
					t_dynamic_table_entry tablaElementos[],
					t_dynamic_table_entry tablaVacios[]){

	int posicionVacioAModificar, posicionElementoAModificar;

	if(hayEspacio(tablaVacios, tamanioTabla, tamanioItem, algoritmo, &posicionVacioAModificar)){

		posicionElementoAModificar = obtenerParticion(tablaElementos, tamanioTabla, 1);
		tablaElementos[posicionElementoAModificar].position = tablaVacios[posicionVacioAModificar].position;
		tablaElementos[posicionElementoAModificar].size = tamanioItem;
		tablaElementos[posicionElementoAModificar].isEmpty = 0;
		tablaElementos[posicionElementoAModificar].id = posicionElementoAModificar; //Provisional, debería ser el id del mensaje
		char* momentoDeCreacion = temporal_get_string_time();
		tablaElementos[posicionElementoAModificar].dateBorn = momentoDeCreacion;
		tablaElementos[posicionElementoAModificar].dateLastUse = momentoDeCreacion;

		modificarTablaVacio(tablaVacios, tamanioItem, posicionVacioAModificar, tamanioParticionMinima);

		setValue(item, tamanioItem, tablaElementos[posicionElementoAModificar].position);
	}
}

void* obtenerItem(int id, t_dynamic_table_entry tablaElementos[]){
	int i = -1;
	do{
		i++;
		if(tablaElementos[i].id == id){
			return getValue(tablaElementos[i].size, tablaElementos[i].position);
		}
	}while(tablaElementos[i].id != id);
}

void eliminarItem(int id, int tamanioTabla, int tamanioParticionMinima,
					t_dynamic_table_entry tablaElementos[],
					t_dynamic_table_entry tablaVacios[]){

	int posDatoAEliminar = obtenerTablaPorId(id, tamanioTabla, tablaElementos);
	int posNuevoVacio = obtenerParticion(tablaVacios, tamanioTabla, 1);
	int espacioVacio = obtenerDesplazamientoMinimo(tamanioParticionMinima, tablaElementos[posDatoAEliminar].size);

	tablaVacios[posNuevoVacio].id = posNuevoVacio;
	tablaVacios[posNuevoVacio].position = tablaElementos[posDatoAEliminar].position;
	tablaVacios[posNuevoVacio].isEmpty = 0;
	tablaVacios[posNuevoVacio].size = espacioVacio;

	tablaElementos[posDatoAEliminar].id = 0;
	tablaElementos[posDatoAEliminar].position = 0;
	tablaElementos[posDatoAEliminar].isEmpty = 1;
	tablaElementos[posDatoAEliminar].size = 0;

}




