#include "buddySystem.h"

#include <ctype.h>
void imprimirBloque(Bloque* bloque)
{
	if(bloque == NULL) return;

	printf("Posición: %d | Tamanio: %d | Tamanio ocupado: %d | Está dividido: %s | LRU: %s | Fecha FIFO: %s\n",
			bloque->posicion, bloque->tamanio, bloque->tamanioOcupado, bloque->estaDividido ? "Si" : "No",
			bloque->fechaUltimoUso == NULL ? "N" : bloque->fechaUltimoUso, bloque->fechaCreacion);

	if(bloque->estaDividido)
	{
		imprimirBloque(bloque->izq);
		imprimirBloque(bloque->der);
	}
}

void imprimirCache()
{
	imprimirBloque(cache);
	fflush(stdout);
}

void crearBloque(Bloque* bloqueActual, int nuevoTamanio)
{
	bloqueActual->estaDividido = 1;
	bloqueActual->tamanioOcupado = 0;
	bloqueActual->fechaCreacion = NULL;
	bloqueActual->fechaUltimoUso = NULL;
	bloqueActual->suscriptoresEnviados = NULL;
	bloqueActual->suscriptoresRecibidos = NULL;
	bloqueActual->izq = (Bloque*)malloc(sizeof(Bloque));
	bloqueActual->der = (Bloque*)malloc(sizeof(Bloque));
	bloqueActual->izq->estaDividido = 0;
	bloqueActual->der->estaDividido = 0;
	bloqueActual->izq->izq = NULL;
	bloqueActual->der->izq = NULL;
	bloqueActual->izq->der = NULL;
	bloqueActual->der->der = NULL;
	bloqueActual->izq->tamanio = nuevoTamanio;
	bloqueActual->der->tamanio = nuevoTamanio;
	bloqueActual->izq->posicion = bloqueActual->posicion;
	bloqueActual->der->posicion = bloqueActual->posicion + nuevoTamanio;
	bloqueActual->izq->tamanioOcupado = 0;
	bloqueActual->der->tamanioOcupado = 0;
	bloqueActual->izq->fechaCreacion = NULL;
	bloqueActual->der->fechaCreacion = NULL;
	bloqueActual->izq->fechaUltimoUso = NULL;
	bloqueActual->der->fechaUltimoUso = NULL;
	bloqueActual->izq->suscriptoresEnviados = NULL;
	bloqueActual->der->suscriptoresEnviados = NULL;
	bloqueActual->izq->suscriptoresRecibidos = NULL;
	bloqueActual->der->suscriptoresRecibidos = NULL;
}

void formarArbol(Bloque* bloque){
	crearBloque(bloque, bloque->tamanio/2);
	if(bloque->izq->tamanio != tamanioParticionMinima){
		formarArbol(bloque->izq);
		formarArbol(bloque->der);
	}
}

void inicializarBuddySystem(int tamanio)
{
	cache = (Cache)malloc(sizeof(Bloque));
	cache->tamanio = tamanio;
	cache->tamanioOcupado = 0;
	cache->posicion = 0;
	cache->estaDividido = 0;
	/*formarArbol(cache);
	imprimirCache();*/
}

void liberarInfoBloque(Bloque* bloqueActual)
{
	if(bloqueActual->fechaCreacion != NULL){
		free(bloqueActual->fechaCreacion);
		bloqueActual->fechaCreacion = NULL;
	}

	if(bloqueActual->fechaUltimoUso != NULL){
		free(bloqueActual->fechaUltimoUso);
		bloqueActual->fechaUltimoUso = NULL;
	}

	if(bloqueActual->suscriptoresEnviados != NULL){
		list_destroy(bloqueActual->suscriptoresEnviados);
		bloqueActual->suscriptoresEnviados = NULL;
	}

	if(bloqueActual->suscriptoresRecibidos != NULL){
		list_destroy(bloqueActual->suscriptoresRecibidos);
		bloqueActual->suscriptoresRecibidos = NULL;
	}
}

void liberarBloque(Bloque* bloqueActual)
{
	if(bloqueActual != NULL)
	{
		if(bloqueActual->estaDividido)
		{
			liberarBloque(bloqueActual->izq);
			liberarBloque(bloqueActual->der);
		}

		//Sólo si tiene un mensaje dentro, va a tener fechas y suscriptores.
		if(bloqueActual->tamanioOcupado > 0)
		{
			liberarInfoBloque(bloqueActual);
		}

		free(bloqueActual);
	}
}

void liberarBuddySystem()
{
	liberarBloque(cache);
}

int dividirBloque(Bloque* bloque, int espacioRequerido)
{
	int tamanioPartes = bloque->tamanio / 2;

	if(tamanioPartes < espacioRequerido || tamanioPartes < tamanioParticionMinima) return 0;

	crearBloque(bloque, tamanioPartes);

	return 1;
}

int calcularEspacio(Bloque* bloque, int espacioRequerido, Bloque** bloqueAModificar)
{
	if(bloque->tamanioOcupado > 0 || bloque->tamanio < espacioRequerido) return 0;

	if(bloque->estaDividido || dividirBloque(bloque, espacioRequerido))
	{
		int obtenidoIzq = calcularEspacio(bloque->izq, espacioRequerido, bloqueAModificar);

		if(obtenidoIzq) return 1;

		int obtenidoDer = calcularEspacio(bloque->der, espacioRequerido, bloqueAModificar);

		return obtenidoDer;
	}
	else
	{
		*bloqueAModificar = bloque;
	}

	return 1;
}

int hayEspacioBuddySystem(int espacioRequerido, Bloque** bloqueAModificar)
{
	return calcularEspacio(cache, espacioRequerido, bloqueAModificar);
}

void eliminarItemBuddySystem(Bloque* bloque)
{
	liberarInfoBloque(bloque);
	bloque->tamanioOcupado = 0;
	bloque->estaDividido = 0;
	bloque->cola = 0;
}

void consolidarBuddySystem(Bloque* bloque, int* reiniciarConsolidacion)
{
	if(bloque->tamanioOcupado > 0 || !bloque->estaDividido) return;

	if(bloque->izq->estaDividido || bloque->der->estaDividido)
	{
		consolidarBuddySystem(bloque->izq, reiniciarConsolidacion);
		consolidarBuddySystem(bloque->der, reiniciarConsolidacion);

		return;
	}

	if(bloque->izq->tamanioOcupado == 0 && bloque->der->tamanioOcupado == 0)
	{
		bloque->estaDividido = 0;
		free(bloque->izq);
		bloque->izq = NULL;
		free(bloque->der);
		bloque->der = NULL;
		*reiniciarConsolidacion = 1;
	}
}

Bloque* calcularFIFOLRUBuddySystem(Bloque* bloque)
{
	if(bloque->estaDividido){

		Bloque* bloqueDeIzquierda = bloque->izq != NULL ? calcularFIFOLRUBuddySystem(bloque->izq) : NULL;
		Bloque* bloqueDeDerecha = bloque->der != NULL ? calcularFIFOLRUBuddySystem(bloque->der) : NULL;
		char* fechaIzquierda = esFIFO ? bloqueDeIzquierda->fechaCreacion : bloqueDeIzquierda->fechaUltimoUso;
		char* fechaDerecha = esFIFO ? bloqueDeDerecha->fechaCreacion : bloqueDeDerecha->fechaUltimoUso;
		if(fechaIzquierda == NULL && fechaDerecha != NULL) //Si la izquierda da NULL, retorna derecha
			return bloqueDeDerecha;
		else if(fechaDerecha == NULL && fechaIzquierda != NULL) //Si la derecha da NULL, retorna izquierda
			return bloqueDeIzquierda;
		else if(esTiempoMasAntiguo(fechaIzquierda,fechaDerecha)) //Si ambos valores no son NULL, se analiza
			return bloqueDeIzquierda;
		else
			return bloqueDeDerecha;
	}
	return bloque; //llega acá cuando llega a una hoja
}

void eliminarVictimaBuddySystem(){

	Bloque* item;

	if(esFIFO || esLRU){
		item = calcularFIFOLRUBuddySystem(cache);
	}
	else
	{
		return;
	}
	log_info(loggerAriel, "Nodo elegido: %s", item->fechaCreacion);
	eliminarItemBuddySystem(item);

	log_info(logger, "CAMBIO");
	imprimirCache();

	int reiniciarConsolidacion;

	do {
		reiniciarConsolidacion = 0;
		consolidarBuddySystem(cache, &reiniciarConsolidacion);
	} while(reiniciarConsolidacion);
}

void agregarElementoValidoBuddySystem(Bloque** bloqueAModificar, void* item, int tamanioItem, long ID, long IDCorrelativo,
		TipoCola cola)
{
	(*bloqueAModificar)->ID = ID;
	(*bloqueAModificar)->IDCorrelativo = IDCorrelativo;
	(*bloqueAModificar)->cola = cola;
	(*bloqueAModificar)->fechaCreacion = temporal_get_string_time();
	(*bloqueAModificar)->fechaUltimoUso = temporal_get_string_time();
	(*bloqueAModificar)->suscriptoresRecibidos = list_create();
	(*bloqueAModificar)->suscriptoresEnviados = list_create();
	(*bloqueAModificar)->tamanioOcupado = tamanioItem;
	//(*bloqueAModificar)->izq = NULL;
	//(*bloqueAModificar)->der = NULL;

	guardarValor(item, tamanioItem, (*bloqueAModificar)->posicion);
}

void agregarItemBuddySystem(void* item, int tamanioItem, long ID, long IDCorrelativo, TipoCola cola)
{
	sem_wait(&mutexCache);

	Bloque* bloqueAModificar;

	int hayEspacioParaItem = hayEspacioBuddySystem(tamanioItem, &bloqueAModificar);

	while(!hayEspacioParaItem)
	{
		eliminarVictimaBuddySystem();
		hayEspacioParaItem = hayEspacioBuddySystem(tamanioItem, &bloqueAModificar);
	}

	agregarElementoValidoBuddySystem(&bloqueAModificar, item, tamanioItem, ID, IDCorrelativo, cola);

	sem_post(&mutexCache);
}

