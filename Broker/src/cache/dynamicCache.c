/*
 * dynamicCache.c
 *
 *  Created on: 25 may. 2020
 *      Author: utnso
 */


#include "dynamicCache.h"


void initializeTable(
		int cacheSize,
		int minPartitionSize,
		t_dynamic_table_entry a[]){
	int tableSize = cacheSize / minPartitionSize;
	t_dynamic_table_entry b [tableSize];
	a = b;
}


//void agregarParticion
