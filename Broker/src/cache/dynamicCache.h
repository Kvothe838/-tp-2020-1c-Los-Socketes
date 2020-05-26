/*
 * dynamicCache.h
 *
 *  Created on: 25 may. 2020
 *      Author: utnso
 */

#ifndef CACHE_DYNAMICCACHE_H_
#define CACHE_DYNAMICCACHE_H_

typedef struct {
	int position;
	int size;
	int id;
	int isEmpty;
} t_dynamic_table_entry;

void initializeTable(int cacheSize, int minPartitionSize, t_dynamic_table_entry a[] );

/*
 *  - agregar part
 *  - leer partic
 *  - eliminar partic
 *  - update part
 */



#endif /* CACHE_DYNAMICCACHE_H_ */
