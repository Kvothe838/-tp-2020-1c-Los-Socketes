/*
 * dynamicCache.c
 *
 *  Created on: 25 may. 2020
 *      Author: utnso
 */


#include "dynamicCache.h"
#include "basicCache.h"

int getBestFit(t_dynamic_table_entry tablaVacios[], int desiredSize) {
	int i = 0;
	int found = -1;
	int bestPosition, bestDifference = 9999999;
	for(i=0; i<tableSize; i++){
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

int hayEspacio(t_dynamic_table_entry tablaVacios[], int espacioRequerido, int *posicion){
	int posEspacioMinimo = 0;
	if(strcmp(algoritmoEleccion, "FF") == 0){
		for(*posicion = 0; *posicion < tableSize; (*posicion)++){
			if(!tablaVacios[*posicion].isEmpty && tablaVacios[*posicion].size >= espacioRequerido)
				return 1;
		}
	}
	else {
		int bestPosition = getBestFit(tablaVacios, espacioRequerido);
		if(bestPosition >= 0){
			*posicion = bestPosition;
			return 1;
		}
		return 0;
	}

	return 0;
}

int obtenerParticion(t_dynamic_table_entry table[], int esVacio){
	for(int i = 0; i < tableSize; i++){
		if(table[i].isEmpty == esVacio)
			return i;
	}
	return -1;
}

void modificarTablaVacio(t_dynamic_table_entry table[],
		int espacioRequerido, int posicionVector, int tamanioParticionMinima){
	int desplazamiento = obtenerDesplazamientoMinimo(tamanioParticionMinima, espacioRequerido);
	if((table[posicionVector].size -= desplazamiento) <= 0){
		table[posicionVector].position = 0;
		table[posicionVector].id = -1;
		table[posicionVector].isEmpty = 1;
		table[posicionVector].size = 0;
		/*
		 * si llega acá es porque el hayEspacio dio true, entonces puede ocurrir el caso
		 * que haya un lugar de espacio que el size de negativo
		 *
		*/
	}
	else{
		table[posicionVector].position += desplazamiento;
	}

}

int obtenerDesplazamientoMinimo(int tamanioParticionMinima, int espacioRequerido){
	int posicionRelativa = 0;
	posicionRelativa = espacioRequerido < tamanioParticionMinima ? tamanioParticionMinima : espacioRequerido;
	//while((posicionRelativa += tamanioParticionMinima) < espacioRequerido);
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
	//table = malloc(tableSize * sizeof(t_dynamic_table_entry));
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

void initializeDataBasic(t_config* config) {
	tamanioCache = config_get_int_value(config, "TAMANO_MEMORIA");
	tamanioParticionMinima = config_get_int_value(config,
			"TAMANO_MINIMO_PARTICION");
	algoritmoEleccion = config_get_string_value(config,
			"ALGORITMO_PARTICION_LIBRE");
}

void agregarElementoValido(int posicionElementoAModificar, int posicionVacioAModificar,
		int tamanioItem, void* item,
		t_dynamic_table_entry tablaElementos[],
		t_dynamic_table_entry tablaVacios[]) {

	posicionElementoAModificar = obtenerParticion(tablaElementos, 1);
	tablaElementos[posicionElementoAModificar].position = tablaVacios[posicionVacioAModificar].position;
	tablaElementos[posicionElementoAModificar].size = tamanioItem;
	tablaElementos[posicionElementoAModificar].isEmpty = 0;
	tablaElementos[posicionElementoAModificar].id = posicionElementoAModificar; //Provisional, debería ser el id del mensaje
	char* momentoDeCreacion = temporal_get_string_time();
	tablaElementos[posicionElementoAModificar].dateBorn = momentoDeCreacion;
	tablaElementos[posicionElementoAModificar].dateLastUse = momentoDeCreacion;
	modificarTablaVacio(tablaVacios, tamanioItem, posicionVacioAModificar,tamanioParticionMinima);
	setValue(item, tamanioItem,	tablaElementos[posicionElementoAModificar].position);


}

void agregarItem(	void* item, int tamanioItem,
					t_dynamic_table_entry tablaElementos[],
					t_dynamic_table_entry tablaVacios[]){

	int posicionVacioAModificar, posicionElementoAModificar;

	if(hayEspacio(tablaVacios, tamanioItem, &posicionVacioAModificar)){
		agregarElementoValido(posicionElementoAModificar, posicionVacioAModificar, tamanioItem, item,
									tablaElementos,	tablaVacios);
	}
	else
	{
		compactarCache(tamanioParticionMinima, tableSize, tablaElementos, tablaVacios);
		if(hayEspacio(tablaVacios, tamanioItem, &posicionVacioAModificar))
		{
			agregarElementoValido(posicionElementoAModificar, posicionVacioAModificar, tamanioItem, item,
							tablaElementos,	tablaVacios);
		}
		else{
			//eliminicación por fifo o lsu
		}
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
	int posNuevoVacio = obtenerParticion(tablaVacios, 1);
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

void compactarCache(int tamanioParticionMinima, int tamanioTabla,
					t_dynamic_table_entry tablaACompactar[],
					t_dynamic_table_entry tablaVacio[])
{
	t_dynamic_table_entry tablaCompactada[tableSize], tablaNuevoVacio[tableSize];
	initializeTable(tamanioCache, tableSize, tablaCompactada, 0);
	initializeTable(tamanioCache, tableSize, tablaNuevoVacio, 1);

	int posicionNueva = 0, posicionVieja;

	for(int i = 0; i < tamanioTabla; i++)
	{
		t_dynamic_table_entry elementoActual = tablaACompactar[i];
		if(!(elementoActual.isEmpty))
		{
			tablaCompactada[i].id = elementoActual.id;
			tablaCompactada[i].size = elementoActual.size;
			tablaCompactada[i].dateBorn = elementoActual.dateBorn;
			tablaCompactada[i].dateLastUse = elementoActual.dateLastUse;
			posicionVieja = elementoActual.position;
			tablaCompactada[i].position = posicionNueva;
			tablaCompactada[i].isEmpty = 0;
			moveBlock(elementoActual.size, posicionVieja, posicionNueva);
			posicionNueva += obtenerDesplazamientoMinimo(tamanioParticionMinima, elementoActual.size);
		}
	}

	/*modificarTablaVacio(t_dynamic_table_entry table[],
		int espacioRequerido, int posicionVector, int tamanioParticionMinima*/
	modificarTablaVacio(tablaNuevoVacio, posicionNueva, 0 /*al principio*/, tamanioParticionMinima);
	memcpy(tablaACompactar, tablaCompactada, sizeof(t_dynamic_table_entry) * tableSize);
	memcpy(tablaVacio, tablaNuevoVacio, sizeof(t_dynamic_table_entry) * tableSize);
	replaceCache(tamanioCache);
}



