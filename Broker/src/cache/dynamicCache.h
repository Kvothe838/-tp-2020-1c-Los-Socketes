/*
 * dynamicCache.h
 *
 *  Created on: 25 may. 2020
 *      Author: utnso
 */

#ifndef CACHE_DYNAMICCACHE_H_
#define CACHE_DYNAMICCACHE_H_


#include <string.h>



typedef struct {
	int position;
	int size;
	int id;
	int isEmpty;
	char* dateBorn;
	char* dateLastUse;
} t_dynamic_table_entry;

void initializeTable(int tamanioCache, int tableSize, t_dynamic_table_entry tabla[], int esVacio);

void agregarItem(	void* item, int tamanioItem, int tamanioTabla, int tamanioParticionMinima,
					char* algoritmoEleccion,
					t_dynamic_table_entry tablaElementos[],
					t_dynamic_table_entry tablaVacios[]);

void eliminarItem(int id, int tamanioTabla, int tamanioParticionMinima,
					t_dynamic_table_entry tablaElementos[],
					t_dynamic_table_entry tablaVacios[]);
/*
 *  - agregar part
 *  - leer partic
 *  - eliminar particint* tableSize
 *  - update part
 */



#endif /* CACHE_DYNAMICCACHE_H_ */
