#ifndef MANEJODEARCHIVOS_H_
#define MANEJODEARCHIVOS_H_

#include <commons/bitarray.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/string.h>
#include <shared/structs.h>
#include <shared/utils.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include "../conexionBinario/conexionBinario.h"

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


char *puntoDeMontaje;
char *numeroMagico;
uint32_t blockCantBits, blockCantBytes, blockSize;
uint32_t reintentoConexion, retardoDeOperacion;

pokemonDatoPosicion aComparar;

void administrarNewPokemon(char* pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad);
uint32_t administrarCatchPokemon(char* pokemon, uint32_t posX, uint32_t posY);
LocalizedPokemon * administrarGetPokemon(char* pokemon);

void liberarVariablesGlobales();
void inicializarData(t_log* logger);

#endif /* MANEJODEARCHIVOS_H_ */
