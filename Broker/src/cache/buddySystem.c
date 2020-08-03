#include "buddySystem.h"

void imprimirBloque(Bloque* bloque)
{
	if(bloque == NULL) return;

	printf("Posición: %d | Tamanio: %d | Tamanio ocupado: %d | Está dividido: %s | LRU: %s\n",
			bloque->posicion, bloque->tamanio, bloque->tamanioOcupado, bloque->estaDividido ? "Si" : "No",
			bloque->fechaUltimoUso == NULL ? "N" : bloque->fechaUltimoUso);

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
	bloqueActual->izq->tamanio = nuevoTamanio;
	bloqueActual->der->tamanio = nuevoTamanio;
	bloqueActual->izq->posicion = bloqueActual->posicion;
	bloqueActual->der->posicion = nuevoTamanio;
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

void inicializarBuddySystem(int tamanio)
{
	cache = (Cache)malloc(sizeof(Bloque));
	cache->tamanio = tamanio;
	cache->tamanioOcupado = 0;
	cache->posicion = 0;
	cache->estaDividido = 0;
}

void liberarInfoBloque(Bloque* bloqueActual)
{
	if(bloqueActual->fechaCreacion != NULL){
		free(bloqueActual->fechaCreacion);
		bloqueActual->fechaCreacion = NULL;
	}

	if(bloqueActual->fechaUltimoUso != NULL){
		free(bloqueActual->fechaUltimoUso);
		bloqueActual->fechaCreacion = NULL;
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

void eliminarItemBuddySystem(Bloque** bloque)
{
	liberarInfoBloque(*bloque);
	(*bloque)->tamanioOcupado = 0;
	(*bloque)->estaDividido = 0;
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

void calcularFIFOLRUBuddySystem(Bloque** bloque, Bloque*** bloqueMasAntiguo)
{
	if((*bloque)->tamanioOcupado > 0)
	{
		if(esTiempoMasAntiguo(esFIFO ? (*bloque)->fechaCreacion : (*bloque)->fechaUltimoUso,
			esFIFO ? (*(*bloqueMasAntiguo))->fechaCreacion : (*(*bloqueMasAntiguo))->fechaUltimoUso))
		{
			*bloqueMasAntiguo = bloque;
		}
	}
	else
	{
		if((*bloque)->estaDividido)
		{
			calcularFIFOLRUBuddySystem(&((*bloque)->izq), bloqueMasAntiguo);
			calcularFIFOLRUBuddySystem(&((*bloque)->der), bloqueMasAntiguo);
		}
	}
}

void eliminarVictimaBuddySystem()
{
	Bloque** item = &cache;

	if(esFIFO || esLRU){
		calcularFIFOLRUBuddySystem(&cache, &item);
	}
	else
	{
		return;
	}

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

