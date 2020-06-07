/*
 * conexionBinario.h
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#ifndef MANEJODEARCHIVOS_H_
#define MANEJODEARCHIVOS_H_

#include <commons/bitarray.h>
#include <commons/log.h>
#include <shared/structs.h>
#include <sys/stat.h>
#include <stdint.h>
#include "../conexionBinario/conexionBinario.h";


typedef struct {
	uint32_t esDirectorio;
	uint32_t tamanio;
	char **bloquesAsociados;
	uint32_t abierto;
} pokemonMetadata;

typedef struct {
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidad;
} pokemonDatoPosicion;

void administrarNewPokemon(char* pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad);
uint32_t administrarCatchPokemon(char* pokemon, uint32_t posX, uint32_t posY);
void inicializarData(t_log* logger);
char* intToString(uint32_t bloque);

#endif /* MANEJODEARCHIVOS_H_ */
