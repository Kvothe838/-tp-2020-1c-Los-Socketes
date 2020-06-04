/*
 * dynamicCache.c
 *
 *  Created on: 25 may. 2020
 *      Author: utnso
 */


#include "dynamicCache.h"
#include "basicCache.h"

int isDateOlder(char* a, char* b){
	return strcmp(a, b) < 0;
}

int getBestFit(t_dynamic_table_entry tablaVacios[], int desiredSize) {
	int i = 0;
	int found = -1;
	int bestPosition, *bestDifference = NULL;
	for(i=0; i<tableSize; i++){
		t_dynamic_table_entry currentEntry = tablaVacios[i];
		if(!currentEntry.isEmpty && currentEntry.size >= desiredSize){
			int diff = currentEntry.size - desiredSize;
			if(bestDifference == NULL || diff < *bestDifference){
				*bestDifference = diff;
				bestPosition = i;
				found = 1;
			}
		}
	}
	return found >= 0 ? bestPosition : found;
}

int calculateFIFO(t_dynamic_table_entry tablaElementos[]){
	int oldestPosition = 0;
	char* oldestDate = "";
	for(int i=0; i<tableSize; i++){
		t_dynamic_table_entry current = tablaElementos[i];
		if(!current.isEmpty && isDateOlder(oldestDate, current.dateBorn)){
			oldestDate = current.dateBorn;
			oldestPosition = i;
		}
	}
	return tablaElementos[oldestPosition].id;
}

int calculateLRU(t_dynamic_table_entry tablaElementos[]){
	int oldestPosition = 0;
	char* oldestDate = "-1:00:00:00";
	for(int i=0; i<tableSize; i++){
		t_dynamic_table_entry current = tablaElementos[i];
		if(!current.isEmpty && isDateOlder(oldestDate, current.dateLastUse)){
			oldestDate = current.dateBorn;
			oldestPosition = i;
		}
	}
	return tablaElementos[oldestPosition].id;
}



void eliminarVictima(t_dynamic_table_entry tablaElementos[], t_dynamic_table_entry tablaVacios[]){
	if(strcmp(algoritmoEleccionDeVictima, "FIFO") == 0){
		eliminarItem(calculateFIFO(tablaElementos), tablaElementos, tablaVacios);
	}
	else{
		eliminarItem(calculateLRU(tablaElementos), tablaElementos, tablaVacios);

	}
}

int hayEspacio(t_dynamic_table_entry tablaVacios[], int espacioRequerido, int *posicion){
	//int posEspacioMinimo = 0;
	if(strcmp(algoritmoEleccionDeParticionLibre, "FF") == 0){
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

int obtenerPrimeraParticion(t_dynamic_table_entry table[], int esVacio){
	for(int i = 0; i < tableSize; i++){
		if(table[i].isEmpty == esVacio)
			return i;
	}
	return -1;
}

int obtenerDesplazamientoMinimo(int tamanioParticionMinima, int espacioRequerido){
	int posicionRelativa = 0;
	posicionRelativa = espacioRequerido < tamanioParticionMinima ? tamanioParticionMinima : espacioRequerido;
	//while((posicionRelativa += tamanioParticionMinima) < espacioRequerido);
	return posicionRelativa;
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

int obtenerTablaPorId(int id, t_dynamic_table_entry tablaElementos[]){
	for(int i = 0; i < tableSize; i++){
		if(tablaElementos[i].id == id)
			return i;
	}
	return -1;
}

void inicializarCache(){
	initializeTable(tamanioCache, tableSize, tablaElementos, 0);
	initializeTable(tamanioCache, tableSize, tablaVacios, 1);
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
	algoritmoEleccionDeParticionLibre = config_get_string_value(config,
			"ALGORITMO_PARTICION_LIBRE");
	algoritmoEleccionDeVictima  = config_get_string_value(config,
			"ALGORITMO_REEMPLAZO");
	frecuenciaCompactacion = (int)config_get_string_value(config,
			"FRECUENCIA_COMPACTACION");

	tableSize = (tamanioCache / tamanioParticionMinima);

	initializeCache(tamanioCache);
}

void agregarElementoValido(int posicionElementoAModificar, int posicionVacioAModificar,
		int tamanioItem, void* item,
		t_dynamic_table_entry tablaElementos[],
		t_dynamic_table_entry tablaVacios[]) {

	posicionElementoAModificar = obtenerPrimeraParticion(tablaElementos, 1);
	tablaElementos[posicionElementoAModificar].position = tablaVacios[posicionVacioAModificar].position;
	tablaElementos[posicionElementoAModificar].size = tamanioItem;
	tablaElementos[posicionElementoAModificar].isEmpty = 0;
	tablaElementos[posicionElementoAModificar].id = posicionElementoAModificar; //Provisional, debería ser el id del mensaje

	tablaElementos[posicionElementoAModificar].dateBorn = temporal_get_string_time();
	tablaElementos[posicionElementoAModificar].dateLastUse = temporal_get_string_time();
	modificarTablaVacio(tablaVacios, tamanioItem, posicionVacioAModificar,tamanioParticionMinima);
	setValue(item, tamanioItem,	tablaElementos[posicionElementoAModificar].position);


}

void agregarItem(void* item, int tamanioItem){

	int posicionVacioAModificar = 1, posicionElementoAModificar = 1;
	//Las inicializo para que no me tire warnings. Ariel te voy a matar.

	if(hayEspacio(tablaVacios, tamanioItem, &posicionVacioAModificar)){
		agregarElementoValido(posicionElementoAModificar, posicionVacioAModificar, tamanioItem, item,
							  tablaElementos, tablaVacios);
	} else {
		int seAgrego = 0;
		for(int i = 0; i < frecuenciaCompactacion && !seAgrego; i++)
		{
			compactarCache(tablaElementos, tablaVacios);
			if(hayEspacio(tablaVacios, tamanioItem, &posicionVacioAModificar)){
				agregarElementoValido(posicionElementoAModificar, posicionVacioAModificar, tamanioItem, item,
								tablaElementos,	tablaVacios);
				seAgrego = 1;
			}
			else
				eliminarVictima(tablaElementos, tablaVacios);
		}
	}
}

void* obtenerItem(long id){
	int i = -1;
	do {
		i++;
		if(tablaElementos[i].id == id){
			free(tablaElementos[i].dateLastUse);
			tablaElementos[i].dateLastUse = temporal_get_string_time();
			return getValue(tablaElementos[i].size, tablaElementos[i].position);
		}
	} while(tablaElementos[i].id != id);

	return NULL;
}

void eliminarItem(int id, t_dynamic_table_entry tablaElementos[], t_dynamic_table_entry tablaVacios[]){

	int posDatoAEliminar = obtenerTablaPorId(id, tablaElementos);
	int posNuevoVacio = obtenerPrimeraParticion(tablaVacios, 1);
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

void compactarCache(t_dynamic_table_entry tablaACompactar[], t_dynamic_table_entry tablaVacio[]){
	t_dynamic_table_entry tablaCompactada[tableSize], tablaNuevoVacio[tableSize];
	initializeTable(tamanioCache, tableSize, tablaCompactada, 0);
	initializeTable(tamanioCache, tableSize, tablaNuevoVacio, 1);

	int posicionNueva = 0, posicionVieja;

	for(int i = 0; i < tableSize; i++)
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
	//replaceCache(tamanioCache);
}



