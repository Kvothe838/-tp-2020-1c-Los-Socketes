#include "buddySystem.h"

void imprimirBloque(Bloque* bloque)
{
	printf("Posición: %d | Tamanio: %d | Tamanio ocupado: %d | Está dividido: %s\n", bloque->posicion, bloque->tamanio, bloque->tamanioOcupado, bloque->estaDividido ? "Si" : "No");
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

void crearBloque(Bloque* bloqueActual)
{
	log_info(logger, "Creo bloque");
	int nuevoTamanio = (int) bloqueActual->tamanio / 2;

	bloqueActual->estaDividido = 1;
	bloqueActual->tamanioOcupado = 0;
	bloqueActual->izq = (Bloque*)malloc(sizeof(Bloque));
	bloqueActual->der = (Bloque*)malloc(sizeof(Bloque));
	bloqueActual->izq->estaDividido = 0;
	bloqueActual->der->estaDividido = 0;
	bloqueActual->izq->tamanio = nuevoTamanio;
	bloqueActual->der->tamanio = nuevoTamanio;
	bloqueActual->izq->posicion = bloqueActual->posicion;
	bloqueActual->der->posicion = nuevoTamanio;
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
	free(bloqueActual->fechaCreacion);
	free(bloqueActual->fechaUltimoUso);
	free(bloqueActual->suscriptoresEnviados);
	free(bloqueActual->suscriptoresRecibidos);
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
	int tamanioPartes = (int) bloque->tamanio / 2;

	log_info(logger, "TAMANIO: %d", tamanioPartes);

	if(tamanioPartes < espacioRequerido) return 0;

	crearBloque(bloque);

	log_info(logger, "Creé el bloque bien: %d %d %d %d", bloque->izq != NULL, bloque->der != NULL, bloque->tamanio, bloque->tamanioOcupado);

	return 1;
}

int calcularFF(Bloque* bloque, int espacioRequerido, Bloque** bloqueAModificar)
{
	log_info(logger, "Bloque | Tamaño: %d | Tamaño ocupado: %d | Está dividido: %d | Requerido: %d\n", bloque->tamanio, bloque->tamanioOcupado, bloque->estaDividido, espacioRequerido);
	if(bloque->tamanioOcupado > 0 || bloque->tamanio < espacioRequerido) return 0;

	if(bloque->estaDividido || dividirBloque(bloque, espacioRequerido))
	{
		log_info(logger, "Consulto izq con tamanio ocupado: %d", bloque->izq->tamanioOcupado);
		int obtenidoIzq = calcularFF(bloque->izq, espacioRequerido, bloqueAModificar);
		log_info(logger, "Termino izq");

		if(obtenidoIzq) return 1;

		log_info(logger, "Consulto der");
		int obtenidoDer = calcularFF(bloque->der, espacioRequerido, bloqueAModificar);
		log_info(logger, "Termino der");

		return obtenidoDer;
	}
	else
	{
		log_info(logger, "Modifico bloque");
		*bloqueAModificar = bloque;
	}

	return 1;
}

int calcularBF(Bloque* bloque, int espacioRequerido, Bloque** bloqueAModificar, int* mejorDiferencia)
{
	if(bloque->tamanioOcupado > 0 || bloque->tamanio <= espacioRequerido) return 0;

	if(bloque->estaDividido || dividirBloque(bloque, espacioRequerido))
	{
		int obtenidoIzq = calcularBF(bloque->izq, espacioRequerido, bloqueAModificar, mejorDiferencia);
		int obtenidoDer = calcularBF(bloque->der, espacioRequerido, bloqueAModificar, mejorDiferencia);

		return obtenidoIzq | obtenidoDer;
	}
	else
	{
		int diferencia = bloque->tamanio - espacioRequerido;

		if(mejorDiferencia == NULL || diferencia < *mejorDiferencia)
		{
			*mejorDiferencia = diferencia;
			*bloqueAModificar = bloque;

			return 1;
		}

		return 0;
	}
}

int hayEspacioBuddySystem(int espacioRequerido, Bloque** bloqueAModificar)
{
	if(esFF)
	{
		return calcularFF(cache, espacioRequerido, bloqueAModificar);
	}
	else if(esBF)
	{
		int *mejorDiferencia = NULL;
		Bloque* mejorBloque;

		int encontrado = calcularBF(cache, espacioRequerido, &mejorBloque, mejorDiferencia);

		if(encontrado)
		{
			*bloqueAModificar = mejorBloque;
		}

		return encontrado;
	}

	return -1;
}

void eliminarItemBuddySystem(Bloque* bloque)
{
	liberarInfoBloque(bloque);
	bloque->tamanioOcupado = 0;
	bloque->estaDividido = 0;
}

void consolidarBuddySystem(Bloque* bloque, int* reiniciarConsolidacion)
{
	if(bloque->tamanioOcupado > 0 || !bloque->estaDividido) return;

	if(bloque->izq->tamanioOcupado == 0 && bloque->der->tamanioOcupado == 0)
	{
		bloque->estaDividido = 0;
		bloque->posicion = bloque->izq->posicion;
		free(bloque->izq);
		free(bloque->der);
		*reiniciarConsolidacion = 1;
	}
	else
	{
		consolidarBuddySystem(bloque->izq, reiniciarConsolidacion);
		consolidarBuddySystem(bloque->der, reiniciarConsolidacion);
	}
}

void calcularFIFOLRUBuddySystem(Bloque* bloque, Bloque** bloqueMasAntiguo, int esFIFO)
{
	if(bloque->tamanioOcupado > 0)
	{
		if(esTiempoMasAntiguo((*bloqueMasAntiguo)->fechaCreacion, esFIFO ? bloque->fechaCreacion : bloque->fechaUltimoUso))
		{
			*bloqueMasAntiguo = bloque;
		}
	}
	else
	{
		if(bloque->estaDividido)
		{
			calcularFIFOLRUBuddySystem(bloque->izq, bloqueMasAntiguo, esFIFO);
			calcularFIFOLRUBuddySystem(bloque->der, bloqueMasAntiguo, esFIFO);
		}
	}
}

void eliminarVictimaBuddySystem()
{
	Bloque* item;

	if(esFIFO){
		calcularFIFOLRUBuddySystem(cache, &item, 1);
	}
	else if(esLRU)
	{
		calcularFIFOLRUBuddySystem(cache, &item, 0);
	}
	else
	{
		return;
	}

	eliminarItemBuddySystem(item);

	int reiniciarConsolidacion;

	do{
		reiniciarConsolidacion = 0;
		consolidarBuddySystem(cache, &reiniciarConsolidacion);
	}while(reiniciarConsolidacion);
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

	guardarValor(item, tamanioItem, (*bloqueAModificar)->posicion);
}

void agregarItemBuddySystem(void* item, int tamanioItem, long ID, long IDCorrelativo, TipoCola cola)
{
	sem_wait(&mutexCache);

	Bloque* bloqueAModificar;

	int hayEspacioParaItem = hayEspacioBuddySystem(tamanioItem, &bloqueAModificar);

	return;

	while(!hayEspacioParaItem)
	{
		eliminarVictimaBuddySystem();
		hayEspacioParaItem = hayEspacioBuddySystem(tamanioItem, &bloqueAModificar);
	}

	agregarElementoValidoBuddySystem(&bloqueAModificar, item, tamanioItem, ID, IDCorrelativo, cola);

	sem_post(&mutexCache);
}

