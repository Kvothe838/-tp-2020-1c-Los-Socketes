#include "manejoDeArchivos.h"
#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <unistd.h>
#include <stdlib.h>


char *ip, *puerto;
t_config *configGameBoy, *metadata;
uint32_t blockCantBits, blockCantBytes, blockSize;
uint32_t reintentoConexion, retardoDeOperacion;

pokemonDatoPosicion * aComparar;


void imprimirNUmero(uint32_t numero){
	printf("Numero %d\n", numero);
}

void imprimirStruct(pokemonDatoPosicion * numero){
	printf("X %d\n", numero->posX);
	printf("Y %d\n", numero->posY);
	printf("Cantidad %d\n", numero->cantidad);
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
	configGameBoy = (t_config*)leer_config("GameCard.config", logger);
	metadata = (t_config*)leer_config("Metadata/metadata.bin", logger);

	reintentoConexion = config_get_int_value(configGameBoy, "TIEMPO_DE_REINTENTO_CONEXION");
	retardoDeOperacion = config_get_int_value(configGameBoy, "TIEMPO_RETARDO_OPERACION");

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

t_list* listNumToStructPos(t_list* listaNumero){
	t_list * listaStruct = list_create();
	t_list * listaTemporal;// = list_create();
	while(!list_is_empty(listaNumero)){
		pokemonDatoPosicion* estructuraALista = malloc(sizeof(pokemonDatoPosicion));
		listaTemporal = list_take_and_remove(listaNumero, 3);
		estructuraALista->posX = (uint32_t)list_get(listaTemporal, 0);
		estructuraALista->posY = (uint32_t)list_get(listaTemporal, 1);
		estructuraALista->cantidad = (uint32_t)list_get(listaTemporal, 2);
		list_add(listaStruct, estructuraALista);
		list_clean(listaTemporal);
	}

	list_destroy_and_destroy_elements(listaNumero, (void*)free);

	return listaStruct;
}

t_list* structPosToListNum(t_list* listaStruct){
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

	list_destroy_and_destroy_elements(listaAuxiliar, (void*)free);
	list_destroy_and_destroy_elements(listaStruct, (void*)free);

	return listaNueva;
}


void modificarArchivos(t_list *listaFinal, char** listaDeBloques){
	t_list* listaNumeros = structPosToListNum(listaFinal);
	uint32_t superaBloque = 0;
	uint32_t pos = 0;
	char path[1000] = "Blocks/";
	strcat(path, listaDeBloques[pos]);
	strcat(path, ".bin");

	FILE* archivo = fopen(path, "rb+");
	uint32_t nroAGuardar;
	while(!list_is_empty(listaNumeros)){
		nroAGuardar = (uint32_t)list_remove(listaNumeros, 0);
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
	sleep(retardoDeOperacion);
}

uint32_t obtenerDataFileSystem(uint32_t tamanio, pokemonMetadata* datosPokemon, t_list** listaFinal) {
	uint32_t tamanioARestar = tamanio;
	uint32_t pos = 0;
	uint32_t espacioLeidoEnBloque = 0;
	uint32_t valor;

	char path[1000] = "Blocks/";
	strcat(path, datosPokemon->bloquesAsociados[pos]);
	strcat(path, ".bin");

	FILE* archivo = fopen(path, "rb");
	t_list* listaValores = list_create();


	while (tamanioARestar != 0) {

		tamanioARestar -= 4;
		espacioLeidoEnBloque += 4;

		fread(&valor, sizeof(uint32_t), 1, archivo);
		list_add(listaValores, valor);

		if (espacioLeidoEnBloque % blockSize == 0 && tamanio != 0) {
			fclose(archivo);
			pos++;
			espacioLeidoEnBloque = 0;
			char path[1000] = "Blocks/";
			strcat(path, datosPokemon->bloquesAsociados[pos]);
			strcat(path, ".bin");
			archivo = fopen(path, "rb");
		}
	}

	*listaFinal = listNumToStructPos(listaValores);

	fclose(archivo);
	return pos;
}



void validarArchivoAbierto(char path[1000], t_config** configPokemon, pokemonMetadata* datosPokemon) {
	do {
		*configPokemon = config_create(path);
		datosPokemon->esDirectorio = (strcmp(config_get_string_value(*configPokemon, "DIRECTORY"), "Y") == 0) ? 1 : 0;
		datosPokemon->tamanio = config_get_int_value(*configPokemon, "SIZE");
		datosPokemon->bloquesAsociados = malloc(1000);
		datosPokemon->bloquesAsociados = config_get_array_value(*configPokemon,	"BLOCKS");
		datosPokemon->abierto =	(strcmp(config_get_string_value(*configPokemon, "OPEN"), "Y") == 0) ? 1 : 0;

		if (!datosPokemon->abierto) {
			config_destroy(*configPokemon);
			sleep(reintentoConexion);
		}
	} while (!datosPokemon->abierto);

	config_set_value(*configPokemon, "OPEN", "N");
	config_save(*configPokemon);
}

void arrayStringToArrayConfig(uint32_t cantidadDeBlocks,
		char nuevoStringBloques[1000], pokemonMetadata* datosPokemon) {
	char** listaString = malloc(100000);
	for (int i = 0; i < cantidadDeBlocks; ++i) {
		listaString[i] = malloc(100);
		strcpy(listaString[i], (datosPokemon->bloquesAsociados)[i]);
	}
	char* numero = malloc(1000);
	strcpy(nuevoStringBloques, "[");
	for (int i = 0; i < cantidadDeBlocks; i++) {
		strcpy(numero, listaString[i]);
		strcat(nuevoStringBloques, numero);
		if (i + 1 != cantidadDeBlocks)
			strcat(nuevoStringBloques, ",");
	}
	strcat(nuevoStringBloques, "]");
	for (int i = 0; i < cantidadDeBlocks; ++i) {
		free(listaString[i]);
	}
	free(listaString);
}

void administrarNewPokemon(char* pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad){
	uint32_t cantidadDeBlocks;
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
		char nuevoStringBloques[1000];
		t_config * configMetadata;

		validarArchivoAbierto(path, &configMetadata, datosPokemon);

		t_list * lista = list_create();
		cantidadDeBlocks = obtenerDataFileSystem(datosPokemon->tamanio, datosPokemon, &lista);

		aComparar = posicionPokemon;

		pokemonDatoPosicion *valorEncontrado = (pokemonDatoPosicion*)list_find(lista, (bool*)mismaPosicion);

		if(valorEncontrado != NULL) //Si encuentra el valor
			valorEncontrado->cantidad += posicionPokemon->cantidad;
		else{
			uint32_t tamanio = datosPokemon->tamanio;
			datosPokemon->tamanio += 12;
			list_add(lista, posicionPokemon);
			if(datosPokemon->tamanio / blockSize > tamanio / blockSize)
			{
				uint32_t nuevoBloque = obtenerBloqueLibre(blockCantBytes);

				cantidadDeBlocks++;

				char str[12];
				snprintf(str, 12, "%d", nuevoBloque);
				datosPokemon->bloquesAsociados[cantidadDeBlocks] = str;

			}
		}
		modificarArchivos(lista, datosPokemon->bloquesAsociados);



		cantidadDeBlocks++; //se le suma una para que las iteraciones se hagas correctamente

		arrayStringToArrayConfig(cantidadDeBlocks, nuevoStringBloques, datosPokemon);

		config_set_value(configMetadata, "OPEN", "Y");
		config_set_value(configMetadata, "BLOCKS", nuevoStringBloques);
		config_set_value(configMetadata, "SIZE", intToString(datosPokemon->tamanio));
		config_save(configMetadata);

		config_destroy(configMetadata);


	}else{
		//SI entra acá es para crear el archivo
		char command[1000] = "mkdir -p Files/Pokemon/";
		strcat(command, pokemon);
		system(command);

		int bloque = obtenerBloqueLibre(blockCantBytes);


		char str[12];
		strcpy(str,intToString(bloque));

		char datosBasico[1000] = "DIRECTORY=N\nSIZE=0\nBLOCKS=[";
		strcat(datosBasico, str);
		strcat(datosBasico, "]\nOPEN=Y");

		FILE* nuevoArchivo = fopen(path, "w");

		fwrite(datosBasico, strlen(datosBasico)+1, 1, nuevoArchivo);
		fclose(nuevoArchivo);
		administrarNewPokemon(pokemon, posX, posY, cantidad);
	}
}


uint32_t administrarCatchPokemon(char* pokemon, uint32_t posX, uint32_t posY){
	t_log* logger = iniciar_logger("GAMECARD", "Catch");
	uint32_t cantidadDeBlocks;
	char path[1000] = "Files/Pokemon/";
	strcat(path, pokemon);


	pokemonDatoPosicion *posicionPokemon = malloc(sizeof(pokemonDatoPosicion));
	posicionPokemon->posX=posX;
	posicionPokemon->posY=posY;
	posicionPokemon->cantidad=1;

	pokemonMetadata* datosPokemon = malloc(sizeof(pokemonMetadata));
	if(access(path, F_OK) != -1){
			char metadataString[] = "/Metadata.bin";
			strcat(path, metadataString);
			t_config * configMetadata;

			validarArchivoAbierto(path, &configMetadata, datosPokemon);

			t_list * lista = list_create();
			cantidadDeBlocks = obtenerDataFileSystem(datosPokemon->tamanio, datosPokemon, &lista);

			aComparar = posicionPokemon;

			pokemonDatoPosicion *valorEncontrado = (pokemonDatoPosicion *)list_find(lista, mismaPosicion);
			if(valorEncontrado != NULL){ //Si encuentra el valor
				char nuevoStringBloques[1000];
				valorEncontrado->cantidad--;

				if(valorEncontrado->cantidad == 0){

					list_remove_and_destroy_by_condition(lista, (bool*)mismaPosicion, (void*)free);

					uint32_t tamanioPrevio = datosPokemon->tamanio;
					datosPokemon->tamanio -= 12;
					if(datosPokemon->tamanio / blockSize < tamanioPrevio / blockSize){
						uint32_t colaAVaciar = atoi(datosPokemon->bloquesAsociados[cantidadDeBlocks]);
						limpiarBloque(colaAVaciar, blockCantBytes);
						cantidadDeBlocks--; //se hace esto para no incluir el último valor del char**
					}

					cantidadDeBlocks++; //se le suma una para que las iteraciones se hagas correctamente

					arrayStringToArrayConfig(cantidadDeBlocks, nuevoStringBloques, datosPokemon);


					config_set_value(configMetadata, "BLOCKS", nuevoStringBloques);
					config_set_value(configMetadata, "SIZE", intToString(datosPokemon->tamanio));
				}

				modificarArchivos(lista, datosPokemon->bloquesAsociados);

				config_set_value(configMetadata, "OPEN", "Y");
				config_save(configMetadata);
				config_destroy(configMetadata);

				return 1;
			}
			else
				log_info(logger, "No se encontró la posición %d - %d del pokemon %s", posX, posY, pokemon);

			config_set_value(configMetadata, "OPEN", "Y");
			config_save(configMetadata);
			config_destroy(configMetadata);
	}
	else
		log_info(logger, "No se encontró el archivo del pokemon %s", pokemon);

	return 0;
}

