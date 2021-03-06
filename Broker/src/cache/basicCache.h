#ifndef CACHE_BASICCACHE_H_
#define CACHE_BASICCACHE_H_

#include <commons/temporal.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <pthread.h>

typedef void* CacheBasica;
CacheBasica cachePrincipal;
CacheBasica cacheAlternativa; //Sólo para compactar.
pthread_mutex_t mutexBasic;

void inicializarCache(int tamanio);
void guardarValor(void* valor, int tamanio, int posicion);
void* obtenerValor(int tamanio, int posicion);
void moverBloque(int tamanio, int posicionVieja, int posicionNueva);
void liberarBasicCache();
void reemplazarCache(int tamanioCache);

#endif
