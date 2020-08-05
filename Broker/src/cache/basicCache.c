#include "basicCache.h"

void inicializarCache(int tamanio){
	pthread_mutex_init(&mutexBasic, NULL);

	cachePrincipal = (CacheBasica*)malloc(tamanio);
	cacheAlternativa = (CacheBasica*)malloc(tamanio);
}

void guardarValor(void* valor, int tamanio, int posicion){
	pthread_mutex_lock(&mutexBasic);
	printf("Puntero a cache: %p\n", cachePrincipal);
	printf("Posición a guardar: %d\n", posicion);
	printf("Tamanio: %d\n", tamanio);
	fflush(stdout);
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
	printf("ANTES ADUGHIAUGHIAU: TAMANIO: %d, POSICION VIEJA: %d, POSICION NUEVA: %d\n", tamanio, posicionVieja, posicionNueva);
	fflush(stdout);
	void* item = obtenerValor(tamanio, posicionVieja);
	printf("DESPUÉS ADGAGAG");
	fflush(stdout);

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

