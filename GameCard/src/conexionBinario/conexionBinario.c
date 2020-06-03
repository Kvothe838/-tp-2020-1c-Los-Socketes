/*
 * conexionBinario.c
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#include<stdio.h>
#include "conexionBinario.h";

void guardarDatos(t_bitarray* bitarray){
    FILE *fout = fopen("Metadata/bitmap.bin", "wb+");
    fwrite(bitarray->bitarray, bitarray->size, 1, fout);
    fclose(fout);
}

t_bitarray* obtenerDatos(int tamanio){
	FILE *fin = fopen("Metadata/bitmap.bin", "rb");
	void* buffer = malloc(tamanio);
	fread(buffer, 1, tamanio, fin);
	fclose(fin);
	return bitarray_create_with_mode(buffer, tamanio, LSB_FIRST);
}
