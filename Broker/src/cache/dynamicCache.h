#ifndef CACHE_DYNAMICCACHE_H_
#define CACHE_DYNAMICCACHE_H_

#include "commons/config.h"
#include <commons/collections/list.h>
#include "shared/structs.h"
#include "shared/commonsExtensions.h"
#include <string.h>
#include <stdlib.h>
#include "basicCache.h"
#include <semaphore.h>
#include "shared/messages.h"

char *algoritmoEleccionDeParticionLibre;
char *algoritmoEleccionDeVictima;
int frecuenciaCompactacion;
int tamanioTabla;
int tamanioCache, tamanioParticionMinima;

t_log* loggerObligatorio;
t_log* loggerInterno;

//sem_t* semCacheTabla;

typedef struct {
	int posicion;
	int tamanio;
	int estaVacio;
	char* fechaCreacion;
	char* fechaUltimoUso;
	long ID;
	long IDCorrelativo;
	TipoCola cola;
	t_list* suscriptoresRecibidos;
	t_list* suscriptoresEnviados;
} ItemTablaDinamica;

ItemTablaDinamica *tablaElementos, *tablaVacios;

void inicializarTabla(ItemTablaDinamica **tabla, int estaVacio);
void inicializarDataBasica(t_config* config, t_log* loggerObligatorioAsignar, t_log* loggerInternoAsignar);
void agregarItem(void* item, int tamanioItem, long ID, long IDCorrelativo, TipoCola cola);
void eliminarItem(long ID);
void inicializarCache();
void* obtenerItem(long id);
void cambiarLRU(long ID);
int obtenerPosicionPorID(long ID);
void agregarSuscriptorEnviado(long IDMensaje, Suscriptor** suscriptor);
void agregarSuscriptorRecibido(long IDMensaje, Suscriptor* suscriptor);
t_list* obtenerSuscriptoresEnviados(long IDMensaje);
int esSuscriptorEnviado(t_list* suscriptoresRecibidos, Suscriptor suscriptor);
int* obtenerTamanioItem(long ID);
long* obtenerIDCorrelativoItem(long ID);
void obtenerDump();
void liberarCache();

#endif /* CACHE_DYNAMICCACHE_H_ */
