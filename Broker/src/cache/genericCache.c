#include "genericCache.h"

void inicializarDataBasica(t_config* config){
	tamanioCache = config_get_int_value(config, "TAMANO_MEMORIA");
	tamanioParticionMinima = (int)config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
	algoritmoMemoria = config_get_string_value(config, "ALGORITMO_MEMORIA");
	algoritmoEleccionDeParticionLibre = config_get_string_value(config,"ALGORITMO_PARTICION_LIBRE");
	algoritmoEleccionDeVictima = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
	frecuenciaCompactacion = config_get_int_value(config, "FRECUENCIA_COMPACTACION");
	tamanioTabla = tamanioCache / tamanioParticionMinima;

	esParticiones = !strcmp(algoritmoMemoria, "PARTICIONES");
	esBS = !strcmp(algoritmoMemoria, "BS");
	esFF = !strcmp(algoritmoEleccionDeParticionLibre, "FF");
	esBF = !strcmp(algoritmoEleccionDeParticionLibre, "BF");
	esFIFO = !strcmp(algoritmoEleccionDeVictima, "FIFO");
	esLRU = !strcmp(algoritmoEleccionDeVictima, "LRU");

	inicializarCache(tamanioCache);

	if(esParticiones)
	{
		pthread_mutex_init(&mutexDynamic, NULL);

		inicializarTabla(&tablaElementos, 0);
		inicializarTabla(&tablaVacios, 1);
	}
	else if(esBS)
	{
		inicializarBuddySystem(tamanioCache);
	}
}

void liberarCache()
{
	if(esParticiones)
	{
		liberarParticiones();
	}
	else
	{
		liberarBuddySystem();
	}
}

int esTiempoMasAntiguo(char* masAntiguo, char* masNuevo){
	return strcmp(masAntiguo, masNuevo) < 0;
}

void agregarItemGeneric(void* item, int tamanioItem, long ID, long IDCorrelativo, TipoCola cola)
{
	if(esParticiones)
	{
		agregarItem(item, tamanioItem, ID, IDCorrelativo, cola);
	} else if(esBS)
	{
		agregarItemBuddySystem(item, tamanioItem, ID, IDCorrelativo, cola);
	}
}

int obtenerPosicionPorIDGeneric(long ID)
{
	int posicion = -1;

	if(esParticiones)
	{
		pthread_mutex_lock(&mutexDynamic);

		posicion = obtenerPosicionPorID(ID);

		pthread_mutex_unlock(&mutexDynamic);
	} else if(esBS) {
		posicion = obtenerPosicionPorIDBuddySystem(ID, cache);
	}

	return posicion;
}

void* obtenerItemGeneric(long ID)
{
	if(esParticiones)
	{
		return obtenerItem(ID);
	}
	else if(esBS)
	{
		return obtenerItemBuddySystem(ID);
	}

	return NULL;
}

int* obtenerTamanioItemGeneric(long ID)
{
	if(esParticiones)
	{
		return obtenerTamanioItem(ID);
	}
	else if(esBS)
	{
		return obtenerTamanioItemBuddySystem(ID);
	}

	return NULL;
}

long* obtenerIDCorrelativoItemGeneric(long ID)
{
	if(esParticiones)
	{
		return obtenerIDCorrelativoItem(ID);
	}
	else if(esBS)
	{
		return obtenerIDCorrelativoItemBuddySystem(ID);
	}

	return NULL;
}

