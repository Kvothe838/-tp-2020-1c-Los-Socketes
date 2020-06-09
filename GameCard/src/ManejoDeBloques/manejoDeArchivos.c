#include "manejoDeArchivos.h"
#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <unistd.h>


char *ip, *puerto;
t_config *configGameBoy, *metadata;
int blockCantBits, blockCantBytes, blockSize;

pokemonDatoPosicion * aComparar;


void imprimirNUmero(uint32_t numero){
	printf("Numero %d\n", numero);
}

void crearBloque(int posicion){
	char str[10];
	sprintf(str, "%d", posicion);

	char path[1000] = "Blocks/";
	strcat(path, str);
	strcat(path, ".bin");

	FILE* nuevoArchivo = fopen(path, "ab+");
	fclose(nuevoArchivo);
}


char* intToString(uint32_t bloque) {
	char* str = malloc(10);
	sprintf(str, "%d", bloque);
	return str;
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
		system("mkdir -p Blocks");
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

bool mismaPosicion(pokemonDatoPosicion* a){
	return (a->posX == aComparar->posX) && (a->posY == aComparar->posY);
}

void listNumToStructPos(t_list* listaNumero){
	t_list* listaNueva = list_create();
	t_list* listaAuxiliar = list_create();
	pokemonDatoPosicion* posNueva;
	while(!list_is_empty(listaNumero)){
		listaAuxiliar = list_take_and_remove(listaNumero, 3);
		posNueva = malloc(sizeof(pokemonDatoPosicion));
		posNueva->posX = list_get(listaAuxiliar, 0);
		posNueva->posY = list_get(listaAuxiliar, 1);
		posNueva->cantidad = list_get(listaAuxiliar, 2);
		list_clean(listaAuxiliar);
		list_add(listaNueva, posNueva);
	}

	list_destroy(listaAuxiliar);

	list_add_all(listaNumero, listaNueva);
}

void structPosToListNum(t_list* listaStruct){
	t_list* listaNueva = list_create();
	t_list* listaAuxiliar = list_create();
	pokemonDatoPosicion* structADeserializar;
	while(!list_is_empty(listaStruct)){
		listaAuxiliar = list_take_and_remove(listaStruct, 1);
		structADeserializar = malloc(sizeof(pokemonDatoPosicion));
		structADeserializar = list_get(listaAuxiliar, 0);
		list_clean(listaAuxiliar);
		list_add(listaNueva, structADeserializar->posX);
		list_add(listaNueva, structADeserializar->posY);
		list_add(listaNueva, structADeserializar->cantidad);
	}

	list_destroy(listaAuxiliar);

	list_add_all(listaStruct, listaNueva);
}

uint32_t obtenerBloqueLibre(){
	t_bitarray *bitmap = obtenerDatos(blockCantBytes);
	uint32_t valor;
	for(uint32_t i = 0; i < 5192/8; i++){
		valor = bitarray_test_bit(bitmap, i);
		if(valor == 0){
			bitarray_set_bit(bitmap, i);
			guardarDatos(bitmap);
			return (i+1);
		}
	}
}

//POR AHORA SIN USO
uint32_t analizandoBloque(uint32_t* tamanio, char path[1000]) {
	if (*tamanio == blockSize) {
		int nuevoBloque = obtenerBloqueLibre();
		char newPath[1000] = "Blocks/";
		strcat(newPath, intToString(nuevoBloque));
		strcat(newPath, ".bin");
		strcpy(path, newPath);
		*tamanio = 0;
		return nuevoBloque;
	}
	return -1;
}

//POR AHORA SIN USO
void redefiniendoBloque(FILE* archivo, pokemonMetadata* datosPokemon, uint32_t *tamanio, uint32_t posActual, char path[1000]) {
	uint32_t posibleNuevoBloque = -1;
	*tamanio += sizeof(uint32_t);
	if ((posibleNuevoBloque = analizandoBloque(tamanio, path)) != -1) {
		fclose(archivo);
		archivo = (path, "a+");
		char str[12];
		strcpy(str,intToString(posibleNuevoBloque));
		datosPokemon->bloquesAsociados[posActual + 1] = str;
	}
}

void modificarArchivos(t_list *listaFinal, uint32_t* listaDeBloques){
	uint32_t superaBloque = 0;
	uint32_t pos = 0;
	char path[1000] = "Blocks/";
	strcat(path, listaDeBloques[pos]);
	strcat(path, ".bin");

	FILE* archivo = fopen(path, "rb+");
	uint32_t nroAGuardar;
	while(!list_is_empty(listaFinal)){
		nroAGuardar = list_remove(listaFinal, 0);
		superaBloque += 4;
		if(superaBloque <= blockSize){
			fwrite(&nroAGuardar, sizeof(uint32_t), 1, archivo);
		}
		else{
			pos++;
			superaBloque = 4;
			char path[1000] = "Blocks/";
			strcat(path, listaDeBloques[pos]);
			strcat(path, ".bin");
			fclose(archivo);
			archivo = fopen(path, "rb+");
			fwrite(&nroAGuardar, sizeof(uint32_t), 1, archivo);
		}
	}
	fclose(archivo);
}

int sumarDatos(pokemonMetadata* datosPokemon, pokemonDatoPosicion* nuevosDatosPosPokemon){
	uint32_t tamanio = datosPokemon->tamanio;
	uint32_t pos = 0;
	uint32_t espacioLeidoEnBloque = 0;
	uint32_t valor;
	uint32_t tamanioComparacion = tamanio;


	char path[1000] = "Blocks/";
	strcat(path, datosPokemon->bloquesAsociados[pos]);
	strcat(path, ".bin");

	t_list * listaValores = list_create();
	FILE* archivo = fopen(path, "rb");

	while(tamanio != 0){
		tamanio -= 4;
		espacioLeidoEnBloque += 4;
		fread(&valor, sizeof(uint32_t), 1, archivo);

		list_add(listaValores, valor);

		if(espacioLeidoEnBloque % blockSize == 0 && tamanio != 0){
			fclose(archivo);
			pos++;
			espacioLeidoEnBloque = 4;
			char path[1000] = "Blocks/";
			strcat(path, datosPokemon->bloquesAsociados[pos]);
			strcat(path, ".bin");

			archivo = fopen(path, "rb");
		}
	}
	fclose(archivo);

	listNumToStructPos(listaValores);

	aComparar = malloc(sizeof(aComparar));
    memcpy(aComparar, nuevosDatosPosPokemon, sizeof(pokemonDatoPosicion));

    pokemonDatoPosicion* ejemplo1 = list_find(listaValores, mismaPosicion);

    if(ejemplo1 != NULL)
    	ejemplo1->cantidad += nuevosDatosPosPokemon->cantidad;
    else{
    	datosPokemon->tamanio += 12;
    	list_add(listaValores, nuevosDatosPosPokemon);
    	if( datosPokemon->tamanio / blockSize > tamanioComparacion / blockSize){
    		pos++;
			uint32_t posBin = obtenerBloqueLibre();
			//memcpy(datosPokemon->bloquesAsociados + (sizeof(uint32_t) * pos), &posBin, sizeof(uint32_t));
			char str[10];
			sprintf(str, "%d", posBin);
			datosPokemon->bloquesAsociados[pos] = str;
    	}
    }

    structPosToListNum(listaValores);

    modificarArchivos(listaValores, datosPokemon->bloquesAsociados);

    return pos + 1;

}

void administrarNewPokemon(char* pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad){
	char path[1000] = "Files/Pokemon/";
	strcat(path, pokemon);

	char metadataString[] = "/Metadata.bin";
	strcat(path, metadataString);

	pokemonDatoPosicion *posicionPokemon = malloc(sizeof(pokemonDatoPosicion));
	posicionPokemon->posX=posX;
	posicionPokemon->posY=posY;
	posicionPokemon->cantidad=cantidad;


	pokemonMetadata* datosPokemon = malloc(sizeof(pokemonMetadata));
	if(access(path, F_OK) != -1){
		//SI entra acá el archivo existe
		t_config* configPokemon = config_create(path);

		datosPokemon->esDirectorio = config_get_int_value(configPokemon, "DIRECTORY") == 'Y' ? 1 : 0;
		datosPokemon->tamanio = config_get_int_value(configPokemon, "SIZE");
		datosPokemon->bloquesAsociados = malloc(1000);
		datosPokemon->bloquesAsociados = config_get_array_value(configPokemon, "BLOCKS");
		datosPokemon->abierto = config_get_int_value(configPokemon, "OPEN") == 'Y' ? 1 : 0;

		int iteraciones = sumarDatos(datosPokemon, posicionPokemon);


		char nuevoStringBloques[1000] = "[";
		for(int i = 0; i < iteraciones; i++){
			strcat(nuevoStringBloques, datosPokemon->bloquesAsociados[i]);
			if(i + 1 != iteraciones)
				strcat(nuevoStringBloques, ",");
		}
		strcat(nuevoStringBloques, "]");

		/*char tamanioCHar[50];
		strcpy(tamanioCHar, intToString(datosPokemon->tamanio));*/


		config_set_value(configPokemon, "BLOCKS", nuevoStringBloques);
		config_set_value(configPokemon, "SIZE", intToString(datosPokemon->tamanio));
		config_save(configPokemon);

		config_destroy(configPokemon);
	}
	else{
		//SI entra acá es para crear el archivo
		char command[1000] = "mkdir -p Files/Pokemon/";
		strcat(command, pokemon);
		system(command);

		int bloque = obtenerBloqueLibre();


		char str[12];
		strcpy(str,intToString(bloque));

		char datosBasico[1000] = "DIRECTORY=N\nSIZE=0\nBLOCKS=[";
		strcat(datosBasico, str);
		strcat(datosBasico, "]\nOPEN=N");

		FILE* nuevoArchivo = fopen(path, "w");

		fwrite(datosBasico, strlen(datosBasico)+1, 1, nuevoArchivo);
		fclose(nuevoArchivo);
		administrarNewPokemon(pokemon, posX, posY, cantidad);
	}
}
