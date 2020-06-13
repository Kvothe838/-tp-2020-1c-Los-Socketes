#include "manejoDeArchivos.h"
#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <unistd.h>
#include <stdlib.h>


//char *ip, *puerto;
char *puntoDeMontaje;
char *numeroMagico;
uint32_t blockCantBits, blockCantBytes, blockSize;
uint32_t reintentoConexion, retardoDeOperacion;

pokemonDatoPosicion aComparar;


void imprimirNUmero(uint32_t numero){
	printf("Numero %d\n", numero);
}

void imprimirStruct(pokemonDatoPosicion * numero){
	printf("X %d\n", numero->posX);
	printf("Y %d\n", numero->posY);
	printf("Cantidad %d\n", numero->cantidad);
}


char* agregarPuntoDeMontaje(char directorio[]){
	char* nuevoValor = malloc(100);
	strcpy(nuevoValor, puntoDeMontaje);
	strcat(nuevoValor, directorio);
	return nuevoValor;
}


char* agregarPuntoDeMontajeDeFile(char directorio[]){
	char* archivoFinal = agregarPuntoDeMontaje("");
	strcat(archivoFinal, numeroMagico);
	strcat(archivoFinal, "/Files/");
	strcat(archivoFinal, directorio);
	return archivoFinal;
}

void crearBloque(int posicion){
	char str[10];
	sprintf(str, "%d", posicion);

	char* path = agregarPuntoDeMontaje("Blocks/");

	if(access(path, F_OK) == -1){
		char instruccion[1000] = "mkdir -p ";
		strcat(instruccion, path);
		system(instruccion);
	}

	strcat(path, str);
	strcat(path, ".bin");

	FILE* nuevoArchivo = fopen(path, "ab+");
	fclose(nuevoArchivo);
	free(path);
}

char* intToString(uint32_t bloque) {
	char* str = malloc(10);
	sprintf(str, "%d", bloque);
	return str;
}

void liberarVariablesGlobales(){

	free(puntoDeMontaje);
	free(numeroMagico);
}

void iniciarMetadataBase(t_config * gameBoy){
	char *bloques, *bloquesTamanio, *magicNumber;
	char* pathMetadataBase = agregarPuntoDeMontaje("Metadata/");

	if(access(pathMetadataBase, F_OK) == -1){

		char instruccion[1000] = "mkdir -p ";
		strcat(instruccion, pathMetadataBase);
		system(instruccion);

		strcat(pathMetadataBase, "Metadata.bin");

		bloques = config_get_string_value(gameBoy, "BLOCKS");
		bloquesTamanio = config_get_string_value(gameBoy, "BLOCK_SIZE");
		magicNumber = config_get_string_value(gameBoy, "MAGIC_NUMBER");
		FILE* archivoMetadataBase = fopen(pathMetadataBase, "w");
		char* valorFinal = malloc(1000);
		sprintf(valorFinal, "BLOCK_SIZE=%s\nBLOCKS=%s\nMAGIC_NUMBER=%s", bloquesTamanio, bloques, magicNumber);

		fwrite(valorFinal, strlen(valorFinal), 1, archivoMetadataBase);
		fclose(archivoMetadataBase);

		free(valorFinal);
	}
	free(pathMetadataBase);
}

void inicializarData(t_log* logger) {
	t_config *configGameBoy, *metadata;

	configGameBoy = leer_config("GameCard.config", logger);

	char* puntoDeMontajeTemporal = config_get_string_value(configGameBoy, "PUNTO_MONTAJE_TALLGRASS");
	puntoDeMontaje = malloc(100);
	strcpy(puntoDeMontaje, puntoDeMontajeTemporal);

	iniciarMetadataBase(configGameBoy);

	char* metadataDireccion = agregarPuntoDeMontaje("Metadata/Metadata.bin");
	metadata = (t_config*)leer_config(metadataDireccion, logger);
	free(metadataDireccion);

	reintentoConexion = config_get_int_value(configGameBoy, "TIEMPO_DE_REINTENTO_CONEXION");
	retardoDeOperacion = config_get_int_value(configGameBoy, "TIEMPO_RETARDO_OPERACION");

	blockCantBits = config_get_int_value(metadata, "BLOCKS");
	blockCantBytes = blockCantBits / 8;
	blockSize = config_get_int_value(metadata, "BLOCK_SIZE");
	char* valorNumeroMagico = config_get_string_value(metadata, "MAGIC_NUMBER");
	numeroMagico = malloc(100);
	strcpy(numeroMagico, valorNumeroMagico);

	char *bitMapDireccion = (char*)agregarPuntoDeMontaje("Metadata/bitmap.bin");

	if(access(bitMapDireccion, F_OK) == -1){

		FILE* nuevoArchivo = fopen(bitMapDireccion, "wb");

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
	free(bitMapDireccion);
	config_destroy(metadata);
	config_destroy(configGameBoy);
}

bool mismaPosicion(pokemonDatoPosicion* a){
	return (a->posX == aComparar.posX) && (a->posY == aComparar.posY);
}


t_list* listNumToStructPos(t_list* listaNumero){
	t_list * listaStruct = list_create();
	t_list * listaTemporal;// = list_create();
	pokemonDatoPosicion *estructuraALista;// = malloc(sizeof(pokemonDatoPosicion));
	while(!list_is_empty(listaNumero)){
		//estructuraALista = malloc(sizeof(pokemonDatoPosicion));
		estructuraALista = malloc(sizeof(pokemonDatoPosicion));
		listaTemporal = list_take_and_remove(listaNumero, 3);
		estructuraALista->posX = (uint32_t)list_get(listaTemporal, 0);
		estructuraALista->posY = (uint32_t)list_get(listaTemporal, 1);
		estructuraALista->cantidad = (uint32_t)list_get(listaTemporal, 2);
		list_add(listaStruct, estructuraALista);
		list_destroy(listaTemporal);
	}
	//free(estructuraALista);

	list_destroy_and_destroy_elements(listaNumero, (void*)free);

	return listaStruct;
}

t_list* structPosToListNum(t_list* listaStruct){
	t_list* listaNueva = list_create();
	pokemonDatoPosicion* structADeserializar;
	while(!list_is_empty(listaStruct)){
		t_list* listaAuxiliar = list_take_and_remove(listaStruct, 1);
		//structADeserializar = malloc(sizeof(pokemonDatoPosicion));
		structADeserializar = list_get(listaAuxiliar, 0);
		list_clean(listaAuxiliar);
		list_add(listaNueva, (structADeserializar->posX));
		list_add(listaNueva, (structADeserializar->posY));
		list_add(listaNueva, (structADeserializar->cantidad));
		list_destroy_and_destroy_elements(listaAuxiliar, (void*)free);
		free(structADeserializar);

	}
	//free(structADeserializar);
	list_destroy_and_destroy_elements(listaStruct, (void*)free);

	return listaNueva;
}


void modificarArchivos(t_list *listaFinal, char** listaDeBloques){
	t_list* listaNumeros = structPosToListNum(listaFinal);
	uint32_t superaBloque = 0;
	uint32_t pos = 0;
	char* path = agregarPuntoDeMontaje("Blocks/");
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
			free(path);
			path = agregarPuntoDeMontaje("Blocks/");
			strcat(path, listaDeBloques[pos]);
			strcat(path, ".bin");
			fclose(archivo);
			archivo = fopen(path, "rb+");
			fwrite(&nroAGuardar, sizeof(uint32_t), 1, archivo);
		}
	}
	free(path);
	fclose(archivo);
	list_destroy_and_destroy_elements(listaNumeros, free);
	//sleep(retardoDeOperacion);
}

uint32_t obtenerDataFileSystem(uint32_t tamanio, pokemonMetadata* datosPokemon, t_list** listaFinal) {
	uint32_t tamanioARestar = tamanio;
	uint32_t pos = 0;
	uint32_t espacioLeidoEnBloque = 0;
	uint32_t valor;

	char *path = agregarPuntoDeMontaje("Blocks/");
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
			free(path);
			path = agregarPuntoDeMontaje("Blocks/");
			strcat(path, datosPokemon->bloquesAsociados[pos]);
			strcat(path, ".bin");
			archivo = fopen(path, "rb");
		}
	}

	*listaFinal = listNumToStructPos(listaValores);

	fclose(archivo);
	free(path);
	return pos;
}

t_config * validarArchivoAbierto(char path[1000], pokemonMetadata* datosPokemon) {
	t_config *configPokemon;
	do {
		configPokemon = config_create(path);
		datosPokemon->esDirectorio = (strcmp(config_get_string_value(configPokemon, "DIRECTORY"), "Y") == 0) ? 1 : 0;
		datosPokemon->tamanio = config_get_int_value(configPokemon, "SIZE");
		//datosPokemon->bloquesAsociados = malloc(1000);
		datosPokemon->bloquesAsociados = config_get_array_value(configPokemon, "BLOCKS");
		datosPokemon->abierto =	(strcmp(config_get_string_value(configPokemon, "OPEN"), "Y") == 0) ? 1 : 0;

		if (!datosPokemon->abierto) {
			config_destroy(configPokemon);
			sleep(reintentoConexion);
		}
	} while (!datosPokemon->abierto);

	config_set_value(configPokemon, "OPEN", "N");
	config_save(configPokemon);
	return configPokemon;
}

void arrayStringToArrayConfig(uint32_t cantidadDeBlocks, char nuevoStringBloques[1000], pokemonMetadata* datosPokemon) {

	char** listaString = malloc(100);
	for (int i = 0; i < cantidadDeBlocks; ++i) {
		listaString[i] = malloc(100);
		strcpy(listaString[i], (datosPokemon->bloquesAsociados)[i]);
	}
	char* numero = malloc(100);
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
	free(numero);
	free(listaString);
}

void administrarNewPokemon(char* pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad){
	uint32_t cantidadDeBlocks;
	char* path = agregarPuntoDeMontajeDeFile("Pokemon/");
	strcat(path, pokemon);




	pokemonMetadata* datosPokemon = malloc(sizeof(pokemonMetadata));
	if(access(path, F_OK) != -1){
		uint32_t hayNuevaPosicion = 0;
		pokemonDatoPosicion *posicionPokemon = malloc(sizeof(pokemonDatoPosicion));
		posicionPokemon->posX=posX;
		posicionPokemon->posY=posY;
		posicionPokemon->cantidad=cantidad;

		strcat(path, "/Metadata.bin");

		char nuevoStringBloques[1000];
		t_config * configMetadata = validarArchivoAbierto(path, datosPokemon);

		t_list * lista;// = list_create();
		cantidadDeBlocks = obtenerDataFileSystem(datosPokemon->tamanio, datosPokemon, &lista);

		aComparar = *posicionPokemon;

		pokemonDatoPosicion *valorEncontrado = (pokemonDatoPosicion*)list_find(lista, (bool*)mismaPosicion);

		if(valorEncontrado != NULL) //Si encuentra el valor
		{
			valorEncontrado->cantidad += posicionPokemon->cantidad;
			free(posicionPokemon);
		}
		else{
			uint32_t tamanio = datosPokemon->tamanio;
			datosPokemon->tamanio += 12;
			list_add(lista, posicionPokemon);
			if(datosPokemon->tamanio / blockSize > tamanio / blockSize)
			{
				uint32_t nuevoBloque = obtenerBloqueLibre(blockCantBytes, puntoDeMontaje);

				cantidadDeBlocks++;
				hayNuevaPosicion++;
				char* str = malloc(10);
				snprintf(str, 10, "%d", nuevoBloque);
				datosPokemon->bloquesAsociados[cantidadDeBlocks] = str;

			}
		}
		modificarArchivos(lista, datosPokemon->bloquesAsociados);


		cantidadDeBlocks++; //se le suma una para que las iteraciones se hagas correctamente

		arrayStringToArrayConfig(cantidadDeBlocks, nuevoStringBloques, datosPokemon);


		config_set_value(configMetadata, "OPEN", "Y");
		config_set_value(configMetadata, "BLOCKS", nuevoStringBloques);
		char* nuevoSize = intToString(datosPokemon->tamanio);
		config_set_value(configMetadata, "SIZE", nuevoSize);
		config_save(configMetadata);
		free(nuevoSize);

		config_destroy(configMetadata);
		cantidadDeBlocks = cantidadDeBlocks > 1 ? (cantidadDeBlocks) : 1;
		for (int i = 0; i < cantidadDeBlocks; ++i) {
			//if((i != cantidadDeBlocks-1 || i == 0))
				free(datosPokemon->bloquesAsociados[i]);
		}
		free(datosPokemon->bloquesAsociados);


	}
	else{
		//SI entra acá es para crear el archivo
		char command[1000] = "mkdir -p ";
		strcat(command, path);
		system(command);

		int bloque = obtenerBloqueLibre(blockCantBytes, puntoDeMontaje);


		char str[12];
		char* copiar = intToString(bloque);
		strcpy(str,copiar);
		free(copiar);

		char datosBasico[1000] = "DIRECTORY=N\nSIZE=0\nBLOCKS=[";
		strcat(datosBasico, str);
		strcat(datosBasico, "]\nOPEN=Y");

		strcat(path, "/Metadata.bin");

		FILE* nuevoArchivo = fopen(path, "w");

		fwrite(datosBasico, strlen(datosBasico)+1, 1, nuevoArchivo);
		fclose(nuevoArchivo);

		free(datosPokemon);
		if(path != NULL)
			free(path);
		administrarNewPokemon(pokemon, posX, posY, cantidad);
		return;

	}
	free(datosPokemon);
	//free(posicionPokemon);
	if(path != NULL)
		free(path);
}


uint32_t administrarCatchPokemon(char* pokemon, uint32_t posX, uint32_t posY){
	t_log* logger = iniciar_logger("GAMECARD.log", "Catch");
	uint32_t cantidadDeBlocks;
	char *path = agregarPuntoDeMontajeDeFile("Pokemon/");
	strcat(path, pokemon);

	pokemonMetadata* datosPokemon = malloc(sizeof(pokemonMetadata));
	if(access(path, F_OK) != -1){
			pokemonDatoPosicion *posicionPokemon = malloc(sizeof(pokemonDatoPosicion));
			posicionPokemon->posX=posX;
			posicionPokemon->posY=posY;
			posicionPokemon->cantidad=1;

			strcat(path, "/Metadata.bin");
			t_config * configMetadata = validarArchivoAbierto(path, datosPokemon);

			t_list * lista;// = list_create();
			cantidadDeBlocks = obtenerDataFileSystem(datosPokemon->tamanio, datosPokemon, &lista);

			aComparar = *posicionPokemon;

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
						limpiarBloque(colaAVaciar, blockCantBytes, puntoDeMontaje);
						cantidadDeBlocks--; //se hace esto para no incluir el último valor del char**
					}

					cantidadDeBlocks++; //se le suma una para que las iteraciones se hagas correctamente

					arrayStringToArrayConfig(cantidadDeBlocks, nuevoStringBloques, datosPokemon);



					config_set_value(configMetadata, "BLOCKS", nuevoStringBloques);

					char* sizeString = intToString(datosPokemon->tamanio);
					config_set_value(configMetadata, "SIZE", sizeString);
					free(sizeString);

				}

				if(!list_is_empty(lista))
					modificarArchivos(lista, datosPokemon->bloquesAsociados);
				else
					list_destroy(lista);


				config_set_value(configMetadata, "OPEN", "Y");
				config_save(configMetadata);

				config_destroy(configMetadata);


				for (int i = 0; i < cantidadDeBlocks; ++i) {
					free(datosPokemon->bloquesAsociados[i]);
				}
				free(datosPokemon->bloquesAsociados);


				log_destroy(logger);
				free(datosPokemon);
				free(path);
				free(posicionPokemon);
				return 1;
			}
			else{
				list_destroy_and_destroy_elements(lista, (void*)free);
				log_info(logger, "No se encontró la posición %d - %d del pokemon %s", posX, posY, pokemon);
			}


			for (int i = 0; i < cantidadDeBlocks+1; ++i) {
				free(datosPokemon->bloquesAsociados[i]);
			}
			free(datosPokemon->bloquesAsociados);

			config_set_value(configMetadata, "OPEN", "Y");
			config_save(configMetadata);
			config_destroy(configMetadata);
			free(posicionPokemon);
	}
	else
		log_info(logger, "No se encontró el archivo del pokemon %s", pokemon);

	log_destroy(logger);
	free(datosPokemon);
	free(path);
	return 0;
}

LocalizedPokemon * administrarGetPokemon(char* pokemon){
	LocalizedPokemon * datosDePosicionPokemon = malloc(sizeof(LocalizedPokemon));

	datosDePosicionPokemon->nombre = pokemon;
	datosDePosicionPokemon->largoNombre = strlen(pokemon);
	datosDePosicionPokemon->cantidadDePosiciones = 0; //para el caso que no encuentre
	datosDePosicionPokemon->data = NULL;

	uint32_t cantidadDeBlocks;
	char *path = agregarPuntoDeMontajeDeFile("Pokemon/");
	strcat(path, pokemon);

	char metadataString[] = "/Metadata.bin";
	strcat(path, metadataString);


	pokemonMetadata* datosPokemon = malloc(sizeof(pokemonMetadata));
	if(access(path, F_OK) != -1){
		t_config * configMetadata = validarArchivoAbierto(path, datosPokemon);

		t_list * lista;// = list_create();
		cantidadDeBlocks = obtenerDataFileSystem(datosPokemon->tamanio, datosPokemon, &lista);

		datosDePosicionPokemon->cantidadDePosiciones = (uint32_t)list_size(lista);

		void* stream = malloc(sizeof(uint32_t) * 3 /* sizeof(uint32_t) * 3 representa una posicion */ * datosDePosicionPokemon->cantidadDePosiciones);

		t_list* listaAuxiliar; //= list_create();
		uint32_t offset = 0;

		while(!list_is_empty(lista)){
			listaAuxiliar = list_take_and_remove(lista, 1);
			pokemonDatoPosicion* structADeserializar; // = malloc(sizeof(pokemonDatoPosicion));
			structADeserializar = list_get(listaAuxiliar, 0);

			memcpy(stream+offset, &(structADeserializar->posX), sizeof(uint32_t));
			offset += sizeof(uint32_t);
			memcpy(stream+offset, &(structADeserializar->posY), sizeof(uint32_t));
			offset += sizeof(uint32_t);
			memcpy(stream+offset, &(structADeserializar->cantidad), sizeof(uint32_t));
			offset += sizeof(uint32_t);

			list_destroy_and_destroy_elements(listaAuxiliar, (void*)free);
		}

		list_destroy_and_destroy_elements(lista, (void*)free);

		datosDePosicionPokemon->data = stream;

		cantidadDeBlocks++;
		for (int i = 0; i < cantidadDeBlocks; ++i) {
			free(datosPokemon->bloquesAsociados[i]);
		}

		free(datosPokemon->bloquesAsociados);

		config_set_value(configMetadata, "OPEN", "Y");
		config_save(configMetadata);

		config_destroy(configMetadata);
	}

	free(datosPokemon);
	free(path);

	return datosDePosicionPokemon;
}

