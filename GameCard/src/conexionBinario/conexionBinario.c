/*
 * conexionBinario.c
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#include<stdio.h>
#include "conexionBinario.h";

void guardarDatos(t_bitarray* bitarray, char* puntoMontaje){
	char *final = malloc(1000);
	strcpy(final, puntoMontaje);
	strcat(final, "Metadata/bitmap.bin");
	FILE *fout = fopen(final, "wb");
    fwrite(bitarray->bitarray, bitarray->size, 1, fout);
	free(final);
    fclose(fout);
}

t_bitarray* obtenerDatos(int tamanio, char* puntoMontaje){
	char *final = malloc(1000);
	strcpy(final, puntoMontaje);
	strcat(final, "Metadata/bitmap.bin");
	FILE *fin = fopen(final, "rb");
	void* buffer = malloc(tamanio);
	fread(buffer, 1, tamanio, fin);
	fclose(fin);
	free(final);
	return bitarray_create_with_mode(buffer, tamanio, LSB_FIRST);
}

uint32_t obtenerBloqueLibre(uint32_t blockCantBytes, char* puntoMontaje){
	t_bitarray *bitmap = obtenerDatos(blockCantBytes, puntoMontaje);
	uint32_t valor;
	for(uint32_t i = 0; i < 5192/8; i++){
		valor = bitarray_test_bit(bitmap, i);
		if(valor == 0){
			bitarray_set_bit(bitmap, i);
			guardarDatos(bitmap, puntoMontaje);
			return (i+1);
		}
	}
}

void limpiarBloque(uint32_t posicion, uint32_t blockCantBytes, char* puntoMontaje){
	t_bitarray *bitmap = obtenerDatos(blockCantBytes, puntoMontaje);
	bitarray_clean_bit(bitmap, posicion-1);
	guardarDatos(bitmap, puntoMontaje);
}

