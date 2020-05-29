/*
 * basicCache.c
 *
 *  Created on: 25 may. 2020
 *      Author: utnso
 */
#include "basicCache.h";

void initializeCache(int size){
	mainCache = malloc(size);
	alternativeCache = malloc(size);
}

void setValue(void* value, int size, int position){
	memcpy(mainCache+position, value, size);
}

void* getValue(int size, int position){
	void* result;
	result = malloc(size);
	memcpy(result, mainCache+position, size);
	return result;
}

void moveBlock(int position, int size, int newPosition){
	void * result = getValue(size, position);
	setValue(result, size, newPosition);
}

