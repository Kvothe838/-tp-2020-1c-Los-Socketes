#include "basicCache.h"

void inicializarCache(int tamanio){
	cachePrincipal = (CacheBasica*)malloc(tamanio);
	cacheAlternativa = (CacheBasica*)malloc(tamanio);
}

void guardarValor(void* valor, int tamanio, int posicion){
	memcpy(cachePrincipal + posicion, valor, tamanio);
}

void* obtenerValor(int tamanio, int posicion){
	void* resultado = malloc(tamanio);
	memcpy(resultado, cachePrincipal + posicion, tamanio);

	return resultado;
}

void moverBloque(int tamanio, int posicionVieja, int posicionNueva){
	void* item = obtenerValor(tamanio, posicionVieja);
	memcpy(cacheAlternativa + posicionNueva, item, tamanio);
}

void reemplazarCache(int tamanioCache){
	memcpy(cachePrincipal, cacheAlternativa, tamanioCache);
}

