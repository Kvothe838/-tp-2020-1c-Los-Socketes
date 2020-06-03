#include "manejoDeArchivos.h"
#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include <unistd.h>


char *ip, *puerto;
t_config *configGameBoy, *metadata;
int blockCantBits, blockCantBytes, blockSize;


void crearBloque(int posicion){
	char str[10];
	sprintf(str, "%d", posicion);

	char path[1000] = "Blocks/";
	strcat(path, str);
	strcat(path, ".bin");

	FILE* nuevoArchivo = fopen(path, "wb");
	fwrite(malloc(blockSize), blockSize, 1, nuevoArchivo);
	fclose(nuevoArchivo);
}

void inicializarData(t_log* logger) {
	configGameBoy = leer_config("GameCard.config", logger);
	metadata = leer_config("Metadata/metadata.bin", logger);
	blockCantBits = config_get_int_value(metadata, "BLOCKS");
	blockCantBytes = blockCantBits / 8;
	blockSize = config_get_int_value(metadata, "BLOCK_SIZE");

	if(access("Metadata/bitmap.bin", F_OK) == -1){
		FILE* nuevoArchivo = fopen("Metadata/bitmap.bin", "wb");

		void* bitmapCont = malloc(blockCantBytes);
		t_bitarray *bitmap = bitarray_create_with_mode(bitmapCont, blockCantBytes, LSB_FIRST);

		for (int var = 0; var < blockCantBits; ++var){
			bitarray_clean_bit(bitmap, var);
			crearBloque(var+1);
		}


		fwrite(bitmap->bitarray, bitmap->size, 1, nuevoArchivo);
		fclose(nuevoArchivo);
		free(bitmapCont);
		bitarray_destroy(bitmap);

	}


}

int obtenerBloqueLibre(){
	t_bitarray *bitmap = obtenerDatos(blockCantBytes);
	int valor;
	for(int i = 0; i < 5192/8; i++){
		valor = bitarray_test_bit(bitmap, i);
		if(valor == 0){
			bitarray_set_bit(bitmap, i);
			guardarDatos(bitmap);
			return (i+1);
		}
	}

}

pokemonMetadata* obtenerMetadatosPokemon(char* pokemon){
	char path[1000] = "Files/Pokemon/";
	strcat(path, pokemon);

	char metadataString[] = "/Metadata.bin";
	strcat(path, metadataString);

	pokemonMetadata* datosPokemon = malloc(sizeof(pokemonMetadata));
	if(access(path, F_OK) != -1){
		t_config* configPokemon = config_create(path);

		datosPokemon->esDirectorio = config_get_int_value(configPokemon, "DIRECTORY") == 'Y' ? 1 : 0;
		datosPokemon->tamanio = config_get_int_value(configPokemon, "SIZE");
		datosPokemon->bloquesAsociados = config_get_array_value(configPokemon, "BLOCKS");
		datosPokemon->abierto = config_get_int_value(configPokemon, "OPEN") == 'Y' ? 1 : 0;

		config_destroy(configPokemon);
		return datosPokemon;
	}
	else{

		char command[1000] = "mkdir -p Files/Pokemon/";
		strcat(command, pokemon);
		system(command);

		int bloque = obtenerBloqueLibre();

		char str[12];
		sprintf(str, "%d", bloque);

		char datosBasico[1000] = "DIRECTORY=N\nSIZE=0\nBLOCKS=[";
		strcat(datosBasico, str);
		strcat(datosBasico, "]\nOPEN=N");

		FILE* nuevoArchivo = fopen(path, "w");

		fwrite(datosBasico, sizeof("DIRECTORY=N\nSIZE=0\nBLOCKS=[n]\nOPEN=Y"), 1, nuevoArchivo);
		fclose(nuevoArchivo);
		return obtenerMetadatosPokemon(pokemon);
	}
	return NULL;
}
