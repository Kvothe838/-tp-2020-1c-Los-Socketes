#include "genericCache.h"

void inicializarDataBasica(t_config* config, t_log* loggerParaAsignar){
	sem_init(&mutexCache, 0, 1);

	tamanioCache = (int)config_get_int_value(config, "TAMANO_MEMORIA");
	tamanioParticionMinima = (int)config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
	algoritmoMemoria = config_get_string_value(config, "ALGORITMO_MEMORIA");
	algoritmoEleccionDeParticionLibre = config_get_string_value(config,"ALGORITMO_PARTICION_LIBRE");
	algoritmoEleccionDeVictima = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
	frecuenciaCompactacion = (int)config_get_string_value(config, "FRECUENCIA_COMPACTACION");
	tamanioTabla = tamanioCache / tamanioParticionMinima;
	logger = loggerParaAsignar;

	esParticiones = !strcmp(algoritmoMemoria, "PARTICIONES");
	esBS = !strcmp(algoritmoMemoria, "BS");
	esFF = !strcmp(algoritmoEleccionDeParticionLibre, "FF");
	esBF = !strcmp(algoritmoEleccionDeParticionLibre, "BF");
	esFIFO = !strcmp(algoritmoEleccionDeVictima, "FIFO");
	esLRU = !strcmp(algoritmoEleccionDeVictima, "LRU");

	inicializarCache(tamanioCache);

	if(esParticiones)
	{
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


