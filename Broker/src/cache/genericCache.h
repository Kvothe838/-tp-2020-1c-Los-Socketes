#ifndef CACHE_GENERICCACHE_H_
#define CACHE_GENERICCACHE_H_

#include "dynamicCache.h"
#include "buddySystem.h"
#include <semaphore.h>

char *algoritmoEleccionDeParticionLibre, *algoritmoEleccionDeVictima, *algoritmoMemoria;
int frecuenciaCompactacion, tamanioCache, tamanioParticionMinima, esParticiones, esBS, esFF, esBF, esFIFO, esLRU;
sem_t mutexCache;
t_log* loggerObligatorio;
t_log* loggerInterno;

void inicializarDataBasica(t_config* config);
void liberarCache();
int esTiempoMasAntiguo(char* masAntiguo, char* masNuevo);

#endif /* CACHE_GENERICCACHE_H_ */
