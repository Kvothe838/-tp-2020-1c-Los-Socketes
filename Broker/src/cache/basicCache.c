#include "basicCache.h"
#include "dynamicCache.h"

void inicializarCache(int tamanio){
	pthread_mutex_init(&mutexBasic, NULL);

	cachePrincipal = (CacheBasica)malloc(tamanio);
	cacheAlternativa = (CacheBasica)malloc(tamanio);
}

void guardarValor(void* valor, int tamanio, int posicion){
	pthread_mutex_lock(&mutexBasic);

	memcpy(cachePrincipal + posicion, valor, tamanio);

	pthread_mutex_unlock(&mutexBasic);
}

void* obtenerValor(int tamanio, int posicion){
	pthread_mutex_lock(&mutexBasic);

	void* resultado = malloc(tamanio);
	memcpy(resultado, cachePrincipal + posicion, tamanio);

	pthread_mutex_unlock(&mutexBasic);

	return resultado;
}

void moverBloque(int tamanio, int posicionVieja, int posicionNueva){
	void* item = obtenerValor(tamanio, posicionVieja);

	pthread_mutex_lock(&mutexBasic);

	memcpy(cacheAlternativa + posicionNueva, item, tamanio);

	pthread_mutex_unlock(&mutexBasic);
}

void reemplazarCache(int tamanioCache){
	pthread_mutex_lock(&mutexBasic);

	memcpy(cachePrincipal, cacheAlternativa, tamanioCache);

	pthread_mutex_unlock(&mutexBasic);
}

void liberarBasicCache()
{
	free(cachePrincipal);
	free(cacheAlternativa);
}

