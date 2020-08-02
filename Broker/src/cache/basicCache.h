#ifndef CACHE_BASICCACHE_H_
#define CACHE_BASICCACHE_H_

#include <commons/temporal.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>

typedef void* CacheBasica;
CacheBasica* cachePrincipal;
CacheBasica* cacheAlternativa; //Sólo para compactar.

void inicializarCache(int tamanio);
void guardarValor(void* valor, int tamanio, int posicion);
void* obtenerValor(int tamanio, int posicion);
void moverBloque(int tamanio, int posicionVieja, int posicionNueva);
void liberarBasicCache();

#endif
