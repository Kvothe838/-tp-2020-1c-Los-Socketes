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
	int nuevoTamanio = bloqueActual->tamanio / 2;

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

	if(bloqueActual->tamanio > tamanioParticionMinima)
	{
		crearBloque(bloqueActual->izq);
		crearBloque(bloqueActual->der);
	}
}

void inicializarBuddySystem()
{
	cache = (Cache)malloc(sizeof(Bloque));
	cache->tamanio = tamanioCache;
	cache->posicion = 0;

	crearBloque(cache);
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
			free(bloqueActual->fechaCreacion);
			free(bloqueActual->fechaUltimoUso);
			free(bloqueActual->suscriptoresEnviados);
			free(bloqueActual->suscriptoresRecibidos);
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

	if(tamanioPartes < espacioRequerido) return 0;

	bloque->estaDividido = 1;
	bloque->izq = malloc(sizeof(Bloque));
}

int calcularFF(Bloque* bloque, int espacioRequerido, Bloque** bloqueAModificar)
{
	if(bloque->tamanioOcupado > 0 || bloque->tamanio <= espacioRequerido) return 0;

	if(bloque->estaDividido || dividirBloque(bloque, espacioRequerido))
	{
		int obtenidoIzq = calcularFF(bloque->izq, espacioRequerido, bloqueAModificar);

		if(obtenidoIzq) return 1;

		int obtenidoDer = calcularFF(bloque->der, espacioRequerido, bloqueAModificar);

		return obtenidoDer;
	}
	else
	{
		*bloqueAModificar = bloque;
	}

	return 1;
}

int calcularBF(Bloque* bloque, int espacioRequerido, Bloque** bloqueAModificar, int* mejorDiferencia)
{
	if(bloque->tamanioOcupado > 0 || bloque->tamanio <= espacioRequerido) return 0;

	if(bloque->estaDividido)
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

		int encontrado = calcularBF(cache, espacioRequerido, &mejorBloque, mejorDiferencia, bloqueAModificar);

		if(encontrado)
		{
			*bloqueAModificar = mejorBloque;
		}

		return encontrado;
	}

	return -1;
}

void eliminarItemBuddySystem(long ID)
{

}

void eliminarVictimaBuddySystem()
{
	if(esFIFO){
		eliminarItemBuddySystem(calcularFIFOBuddySystem());
	}
	else if(esLRU)
	{
		eliminarItemBuddySystem(calcularLRUBuddySystem());
	}
}

void agregarElementoValidoBuddySystem(Bloque** bloqueAModificar, void* item, int tamanioItem, long ID, long IDCorrelativo,
		TipoCola cola)
{
	(*bloqueAModificar)->ID = ID;
	(*bloqueAModificar)->IDCorrelativo = IDCorrelativo;
	(*bloqueAModificar)->cola = cola;
	(*bloqueAModificar)->fechaCreacion = temporal_get_string_time;
	(*bloqueAModificar)->fechaUltimoUso = temporal_get_string_time;
	(*bloqueAModificar)->suscriptoresRecibidos = list_create();
	(*bloqueAModificar)->suscriptoresEnviados = list_create();

	guardarValor(item, tamanioItem, (*bloqueAModificar)->posicion);
}


void agregarItem(void* item, int tamanioItem, long ID, long IDCorrelativo, TipoCola cola)
{
	sem_wait(&mutexCache);

	Bloque** bloqueAModificar;

	int hayEspacioParaItem = hayEspacioBuddySystem(tamanioItem, bloqueAModificar, &bloqueAModificar);

	while(!hayEspacioParaItem)
	{
		eliminarVictimaBuddySystem();
		hayEspacioParaItem = hayEspacioBuddySystem(tamanioItem, bloqueAModificar);
	}

	agregarElementoValidoBuddySystem(bloqueAModificar, item, tamanioItem, ID, IDCorrelativo, cola);

	sem_post(&mutexCache);
}

