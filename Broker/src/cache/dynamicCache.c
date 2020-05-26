/*
 * dynamicCache.c
 *
 *  Created on: 25 may. 2020
 *      Author: utnso
 */


#include "dynamicCache.h"

int hayEspacio(t_dynamic_table_entry tablaVacios[], int tamanioTabla, int espacioRequerido, int *posicion){
	for(*posicion = 0; *posicion < tamanioTabla; (*posicion)++){
		if(!tablaVacios[*posicion].isEmpty && tablaVacios[*posicion].size > espacioRequerido)
			return 1;
	}
	return 0;
}

void modificarTablaVacio(t_dynamic_table_entry table[], int espacioRequerido, int posicionVector, int tamanioParticionMinima){
	table[posicionVector].size -= tamanioParticionMinima;
	int posicionNueva = 0;
	while((posicionNueva += tamanioParticionMinima) < espacioRequerido);

	table[posicionVector].position += posicionNueva;
}

int obtenerParticionVacia(t_dynamic_table_entry table[], int tamanioTabla){
	for(int i = 0; i < tamanioTabla; i++){
		if(table[i].isEmpty == 1)
			return i;
	}
	return -1;
}

void initializeTable(int tamanioCache, int tableSize, t_dynamic_table_entry table[], int esVacio){
	if(esVacio){
		table[0].id = 1;
		table[0].position= 0;
		table[0].size = tamanioCache;
		table[0].isEmpty = 0;
	}
	for(int i = esVacio; i < tableSize; i++){
		table[i].id = 0;
		table[i].position= 0;
		table[i].size = 0;
		table[i].isEmpty = 1;
	}
}

void agregarItem(	void* item, int tamanioItem, int tamanioTabla, int tamanioParticionMinima,
					t_dynamic_table_entry tablaElementos[],
					t_dynamic_table_entry tablaVacios[]){

	int posicionVacioAModificar, posicionElementoAModificar;

	if(hayEspacio(tablaVacios, tamanioTabla, tamanioItem, &posicionVacioAModificar)){

		posicionElementoAModificar = obtenerParticionVacia(tablaElementos, tamanioTabla);
		tablaElementos[posicionElementoAModificar].position = tablaVacios[posicionVacioAModificar].position;
		tablaElementos[posicionElementoAModificar].size = tamanioItem;
		tablaElementos[posicionElementoAModificar].isEmpty = 0;
		tablaElementos[posicionElementoAModificar].id = posicionElementoAModificar; //Provisional, debería ser el id del mensaje

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


//void agregarParticion
