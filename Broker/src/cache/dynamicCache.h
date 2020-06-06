#ifndef CACHE_DYNAMICCACHE_H_
#define CACHE_DYNAMICCACHE_H_

#include "commons/config.h"
#include <string.h>
#include <stdlib.h>
#include "basicCache.h"

char *algoritmoEleccionDeParticionLibre;
char *algoritmoEleccionDeVictima;
int frecuenciaCompactacion;
int tableSize;
int tamanioCache, tamanioParticionMinima;

typedef struct {
	int position;
	int size;
	long id;
	int isEmpty;
	char* dateBorn;
	char* dateLastUse;
} DynamicTableEntry;

DynamicTableEntry *tablaElementos, *tablaVacios; //contienen elementos del tipo DynamicTableEntry

void inicializarTabla(DynamicTableEntry **tabla, int esVacio);
void inicializarDataBasica(t_config* config, t_log* logger);
void agregarItem(void* item, int tamanioItem, int id);
void eliminarItem(long id);
void compactarCache();
void eliminarVictima();
void inicializarCache();
void* obtenerItem(long id);

#endif /* CACHE_DYNAMICCACHE_H_ */
