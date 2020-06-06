#include "dynamicCache.h"

int esTiempoMasAntiguo(char* masAntiguo, char* masNuevo){
	return strcmp(masAntiguo, masNuevo) < 0;
}

int calcularBestFit(int desiredSize) {
	int found = -1;
	int bestPosition, *bestDifference = NULL;

	for(int i = 0; i < tableSize; i++){
		DynamicTableEntry currentEntry = tablaVacios[i];

		if(!currentEntry.isEmpty && currentEntry.size >= desiredSize){
			int diff = currentEntry.size - desiredSize;
			if(bestDifference >= 0 && (bestDifference == NULL || diff < *bestDifference)){
				*bestDifference = diff;
				bestPosition = i;
				found = 1;
			}
		}
	}

	return found >= 0 ? bestPosition : found;
}

int calcularFIFO(){
	int oldestPosition = 0;
	char* oldestDate = "";

	for(int i = 0; i < tableSize; i++){
		DynamicTableEntry current = tablaElementos[i];

		if(!current.isEmpty && esTiempoMasAntiguo(oldestDate, current.dateBorn)){
			oldestDate = current.dateBorn;
			oldestPosition = i;
		}
	}

	return tablaElementos[oldestPosition].id;
}

int calcularLRU(){
	int oldestPosition = 0;
	char* oldestDate = tablaElementos[0].dateBorn;

	if(tableSize > 1){
		for(int i = 1; i < tableSize; i++){
			DynamicTableEntry current = tablaElementos[i];

			if(!current.isEmpty && esTiempoMasAntiguo(oldestDate, current.dateLastUse)){
				oldestDate = current.dateBorn;
				oldestPosition = i;
			}
		}
	}

	return tablaElementos[oldestPosition].id;
}

void eliminarVictima(){
	if(strcmp(algoritmoEleccionDeVictima, "FIFO") == 0){
		eliminarItem(calcularFIFO());
	} else{
		eliminarItem(calcularLRU());
	}
}

int hayEspacio(int espacioRequerido, int *posicion){
	//int posEspacioMinimo = 0;
	if(strcmp(algoritmoEleccionDeParticionLibre, "FF") == 0){
		for(*posicion = 0; *posicion < tableSize; (*posicion)++){
			if(!tablaVacios[*posicion].isEmpty && tablaVacios[*posicion].size >= espacioRequerido)
				return 1;
		}
	} else {
		int mejorPosicion = calcularBestFit(espacioRequerido);

		if(mejorPosicion >= 0){
			*posicion = mejorPosicion;
			return 1;
		}

		return 0;
	}

	return 0;
}

int obtenerPrimeraParticion(DynamicTableEntry tabla[], int esVacio){
	for(int i = 0; i < tableSize; i++){
		if(tabla[i].isEmpty == esVacio)
			return i;
	}

	return -1;
}

int obtenerDesplazamientoMinimo(int tamanioParticionMinima, int espacioRequerido){
	return espacioRequerido < tamanioParticionMinima ? tamanioParticionMinima : espacioRequerido;
}

void modificarTablaVacio(DynamicTableEntry *tabla, int espacioRequerido, int posicionVector,
		int tamanioParticionMinima){
	int desplazamiento = obtenerDesplazamientoMinimo(tamanioParticionMinima, espacioRequerido);
	if((tabla[posicionVector].size -= desplazamiento) <= 0){
		tabla[posicionVector].position = 0;
		tabla[posicionVector].id = -1;
		tabla[posicionVector].isEmpty = 1;
		tabla[posicionVector].size = 0;
		/*
		 * si llega acá es porque el hayEspacio dio true, entonces puede ocurrir el caso
		 * que haya un lugar de espacio que el size de negativo
		 *
		*/
	}
	else{
		tabla[posicionVector].position += desplazamiento;
	}
}

int obtenerPosicionPorId(int id){
	for(int i = 0; i < tableSize; i++){
		if(tablaElementos[i].id == id)
			return i;
	}

	return -1;
}

void inicializarTabla(DynamicTableEntry **tabla, int esVacio){
	*tabla = (DynamicTableEntry*)malloc(tableSize * sizeof(DynamicTableEntry));

	if(esVacio){
		(*tabla)[0].id = 0;
		(*tabla)[0].position = 0;
		(*tabla)[0].size = tamanioCache;
		(*tabla)[0].isEmpty = 0;
	}

	for(int i = esVacio; i < tableSize; i++){
		(*tabla)[i].id = -1;
		(*tabla)[i].position = 0;
		(*tabla)[i].size = 0;
		(*tabla)[i].isEmpty = 1;
	}
}

void inicializarCache(){
	inicializarTabla(&tablaElementos, 0);
	inicializarTabla(&tablaVacios, 1);
}

void inicializarDataBasica(t_config* config, t_log* loggerParaAsignar) {
	tamanioCache = (int)config_get_int_value(config, "TAMANO_MEMORIA");
	tamanioParticionMinima = (int)config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
	algoritmoEleccionDeParticionLibre = config_get_string_value(config,"ALGORITMO_PARTICION_LIBRE");
	algoritmoEleccionDeVictima = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
	frecuenciaCompactacion = (int)config_get_string_value(config, "FRECUENCIA_COMPACTACION");
	tableSize = tamanioCache / tamanioParticionMinima;
	logger = loggerParaAsignar;

	inicializarCache();
}

void agregarElementoValido(int posicionVacioAModificar, int tamanioItem, void* item, int id) {
	int posicionElementoAModificar = obtenerPrimeraParticion(tablaElementos, 1);
	tablaElementos[posicionElementoAModificar].position = tablaVacios[posicionVacioAModificar].position;
	tablaElementos[posicionElementoAModificar].size = tamanioItem;
	tablaElementos[posicionElementoAModificar].isEmpty = 0;
	tablaElementos[posicionElementoAModificar].id = id; //Provisional, debería ser el id del mensaje

	tablaElementos[posicionElementoAModificar].dateBorn = temporal_get_string_time();
	tablaElementos[posicionElementoAModificar].dateLastUse = temporal_get_string_time();
	modificarTablaVacio(tablaVacios, tamanioItem, posicionVacioAModificar,tamanioParticionMinima);
	setValue(item, tamanioItem,	tablaElementos[posicionElementoAModificar].position);
}

void agregarItem(void* item, int tamanioItem, int id){
	int posicionVacioAModificar;
	//Las inicializo para que no me tire warnings. Ariel te voy a matar.

	if(hayEspacio(tamanioItem, &posicionVacioAModificar)){
		agregarElementoValido(posicionVacioAModificar, tamanioItem, item, id);
	} else {
		int seAgrego = 0;
		for(int i = 0; i < frecuenciaCompactacion && !seAgrego; i++)
		{
			compactarCache();
			if(hayEspacio(tamanioItem, &posicionVacioAModificar)){
				agregarElementoValido(posicionVacioAModificar, tamanioItem, item, id);
				seAgrego = 1;
			}
			else
				eliminarVictima();
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

void eliminarItem(long id){
	int posDatoAEliminar = obtenerPosicionPorId(id);
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

void compactarCache(){
	DynamicTableEntry *tablaCompactada = NULL, *tablaNuevoVacio = NULL;
	int posicionNueva = 0, posicionVieja;

	inicializarTabla(&tablaCompactada, 0);
	inicializarTabla(&tablaNuevoVacio, 1);

	for(int i = 0; i < tableSize; i++)
	{
		DynamicTableEntry elementoActual = tablaElementos[i];

		if(!elementoActual.isEmpty)
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

	modificarTablaVacio(tablaNuevoVacio, posicionNueva, 0 /*al principio*/, tamanioParticionMinima);
	memcpy(tablaElementos, tablaCompactada, sizeof(DynamicTableEntry) * tableSize);
	memcpy(tablaVacios, tablaNuevoVacio, sizeof(DynamicTableEntry) * tableSize);
}



