#ifndef CACHE_DYNAMICCACHE_H_
#define CACHE_DYNAMICCACHE_H_

#include "commons/config.h"
#include <string.h>
#include <stdlib.h>
#include "basicCache.h"

char *algoritmoEleccionDeParticionLibre;
char *algoritmoEleccionDeVictima;
int frecuenciaCompactacion;
int tamanioTabla;
int tamanioCache, tamanioParticionMinima;

typedef struct {
	int posicion;
	int tamanio;
	long ID;
	int estaVacio;
	char* fechaCreacion;
	char* fechaUltimoUso;
} ItemTablaDinamica;

ItemTablaDinamica *tablaElementos, *tablaVacios;

void inicializarTabla(ItemTablaDinamica **tabla, int esVacio);
void inicializarDataBasica(t_config* config, t_log* logger);
void agregarItem(void* item, int tamanioItem, int id);
void eliminarItem(long id);
void compactarCache();
void eliminarVictima();
void inicializarCache();
void* obtenerItem(long id);
void imprimirTabla(ItemTablaDinamica tabla[], t_log* logger);

#endif /* CACHE_DYNAMICCACHE_H_ */
