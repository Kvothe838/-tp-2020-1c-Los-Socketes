/*
 * conexionBinario.h
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#ifndef MANEJODEARCHIVOS_H_
#define MANEJODEARCHIVOS_H_

#include <commons/bitarray.h>
#include <sys/stat.h>
#include <stdint.h>
#include "../conexionBinario/conexionBinario.h"
#include <commons/log.h>

typedef struct {
	uint32_t esDirectorio;
	uint32_t tamanio;
	uint32_t *bloquesAsociados;
	uint32_t abierto;
} pokemonMetadata;

typedef struct {
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidad;
} pokemonDatoPosicion;

void administrarNewPokemon(char* pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad);
void inicializarData(t_log* logger);
char* intToString(uint32_t bloque);

#endif /* MANEJODEARCHIVOS_H_ */
