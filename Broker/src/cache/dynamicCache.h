/*
 * dynamicCache.h
 *
 *  Created on: 25 may. 2020
 *      Author: utnso
 */

#ifndef CACHE_DYNAMICCACHE_H_
#define CACHE_DYNAMICCACHE_H_

char *algoritmoEleccionDeParticionLibre;
char *algoritmoEleccionDeVictima;
int frecuenciaCompactacion;
int tableSize;
int tamanioCache, tamanioParticionMinima;

#include "commons/config.h";



typedef struct {
	int position;
	int size;
	int id;
	int isEmpty;
	char* dateBorn;
	char* dateLastUse;
} t_dynamic_table_entry;

void initializeTable(int tamanioCache, int tableSize,
		t_dynamic_table_entry table[], int esVacio);

void initializeDataBasic(t_config* config);

void agregarItem(	void* item, int tamanioItem,
					t_dynamic_table_entry tablaElementos[],
					t_dynamic_table_entry tablaVacios[]);

void eliminarItem(int id,
					t_dynamic_table_entry tablaElementos[],
					t_dynamic_table_entry tablaVacios[]);

void compactarCache(t_dynamic_table_entry tablaACompactar[],
					t_dynamic_table_entry tablaFinal[]);

void eliminarVictima(t_dynamic_table_entry tablaElementos[], t_dynamic_table_entry tablaVacios[]);

/*
 *  - agregar part
 *  - leer partic
 *  - eliminar particint* tableSize
 *  - update part
 */



#endif /* CACHE_DYNAMICCACHE_H_ */
