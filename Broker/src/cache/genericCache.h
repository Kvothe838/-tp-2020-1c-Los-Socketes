#ifndef CACHE_GENERICCACHE_H_
#define CACHE_GENERICCACHE_H_

#include "dynamicCache.h"
#include "buddySystem.h"
#include <semaphore.h>

char *algoritmoEleccionDeParticionLibre, *algoritmoEleccionDeVictima, *algoritmoMemoria;
int frecuenciaCompactacion, tamanioCache, tamanioParticionMinima, esParticiones, esBS, esFF, esBF, esFIFO, esLRU;
t_log* loggerObligatorio;
t_log* loggerInterno;

void inicializarDataBasica(t_config* config);
void liberarCache();
int esTiempoMasAntiguo(char* masAntiguo, char* masNuevo);
void agregarItemGeneric(void* item, int tamanioItem, long ID, long IDCorrelativo, TipoCola cola);
int obtenerPosicionPorIDGeneric(long ID);
void* obtenerItemGeneric(long ID);
int* obtenerTamanioItemGeneric(long ID);
long* obtenerIDCorrelativoItemGeneric(long ID);

#endif /* CACHE_GENERICCACHE_H_ */
