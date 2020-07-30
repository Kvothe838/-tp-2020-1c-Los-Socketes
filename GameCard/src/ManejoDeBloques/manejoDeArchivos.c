#include "manejoDeArchivos.h"

pokemonDatoPosicion* obtenerDataDePosicion(char* stringData){
	char** primeraSeparacion = string_split(stringData, "-");
	char** segundaSeparacion = string_split(primeraSeparacion[1], "=");

	pokemonDatoPosicion* devolucion = malloc(sizeof(pokemonDatoPosicion));
	devolucion->posX = atoi(primeraSeparacion[0]);
	devolucion->posY = atoi(segundaSeparacion[0]);
	devolucion->cantidad = atoi(segundaSeparacion[1]);

	free(primeraSeparacion[0]);
	free(primeraSeparacion[1]);
	free(segundaSeparacion[0]);
	free(segundaSeparacion[1]);
	free(primeraSeparacion);
	free(segundaSeparacion);

	return devolucion;
}

t_list * obtenerDataDeVariasPosiciones(char* stringDatas){
	char** separaciones = string_split(stringDatas, "\n");
	t_list * listaNueva = list_create();
	int i = 0;
	while(separaciones[i] != NULL /* && separaciones[i] != "" Usar strcmp (o algo así) porque sino el compilador chilla*/){
		pokemonDatoPosicion* nuevaData = obtenerDataDePosicion(separaciones[i]);
		list_add(listaNueva, nuevaData);
		free(separaciones[i]);
		i++;
		//printf("El dato es '%s'", separaciones[i]);

	}
	free(separaciones);
	return listaNueva;
}

char * generarNuevaLinea(pokemonDatoPosicion* data){
	char* nuevaLinea = string_from_format("%d-%d=%d\n", data->posX, data->posY, data->cantidad);
	return nuevaLinea;
}
char* agregarPuntoDeMontaje(char directorio[]){
	char* nuevoValor = malloc(100);
	strcpy(nuevoValor, puntoDeMontaje);
	//strcat(nuevoValor, numeroMagico);
	strcat(nuevoValor, "/");
	strcat(nuevoValor, directorio);
	return nuevoValor;
}


char* agregarPuntoDeMontajeConMagicNumber(char directorio[]){
	char* archivoFinal = agregarPuntoDeMontaje("");
	strcat(archivoFinal, numeroMagico);
	strcat(archivoFinal, "/");
	strcat(archivoFinal, directorio);
	return archivoFinal;
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

void crearBloque(int posicion){
	char str[10];
	sprintf(str, "%d", posicion);

	char* path = agregarPuntoDeMontajeConMagicNumber("Blocks/");

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


int mismaPosicion(pokemonDatoPosicion* a){
	return (a->posX == aComparar.posX) && (a->posY == aComparar.posY);
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

	char* metadataArchivo = agregarPuntoDeMontaje("Metadata.bin");
	if(access(metadataArchivo, F_OK) == -1){
		FILE* nuevoArchivo = fopen(metadataArchivo, "w+");
		fwrite("DIRECTORY=Y", strlen("DIRECTORY=Y"), 1, nuevoArchivo);
		fclose(nuevoArchivo);
	}
	free(metadataArchivo);

	metadataArchivo = agregarPuntoDeMontajeConMagicNumber("Metadata.bin");
	if(access(metadataArchivo, F_OK) == -1){
		FILE* nuevoArchivo = fopen(metadataArchivo, "w+");
		fwrite("DIRECTORY=Y", strlen("DIRECTORY=Y"), 1, nuevoArchivo);
		fclose(nuevoArchivo);
	}
	free(metadataArchivo);

	metadataArchivo = agregarPuntoDeMontajeConMagicNumber("Files/");

	char command[1000] = "mkdir -p ";
	strcat(command, metadataArchivo);
	system(command);

	strcat(metadataArchivo, "Metadata.bin");
	if(access(metadataArchivo, F_OK) == -1){
		FILE* nuevoArchivo = fopen(metadataArchivo, "w+");
		fwrite("DIRECTORY=Y", strlen("DIRECTORY=Y"), 1, nuevoArchivo);
		fclose(nuevoArchivo);
	}
	free(metadataArchivo);

}

void liberarVariablesGlobales(){

	free(puntoDeMontaje);
	free(numeroMagico);
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
	config_destroy(configPokemon);
	configPokemon = config_create(path);
	return configPokemon;
}

uint32_t obtenerDataFileSystem(uint32_t const tamanio, char** listaBloques, t_list** listaFinal) {
	uint32_t tamanioARestar = tamanio;
	uint32_t pos = 0;
	uint32_t offset = 0;


	char dataDeArchivoEnRAM[tamanio + 1];
	memset(dataDeArchivoEnRAM,0,tamanio);

	char *path = agregarPuntoDeMontajeConMagicNumber("Blocks/");
	strcat(path, listaBloques[pos]);
	strcat(path, ".bin");



	//char* datoACopiar;//  = malloc(blockSize);
	while (tamanioARestar != 0) {
		FILE* archivo = fopen(path, "rb");

		if(tamanioARestar > blockSize){
			fread(dataDeArchivoEnRAM + offset, blockSize, 1, archivo);
			fclose(archivo);


			offset += blockSize;

			pos++;
			free(path);
			path = agregarPuntoDeMontajeConMagicNumber("Blocks/");
			strcat(path, listaBloques[pos]);
			strcat(path, ".bin");

			tamanioARestar -= blockSize;
		}
		else{
			fread(dataDeArchivoEnRAM + offset, tamanioARestar, 1, archivo);
			fclose(archivo);
			tamanioARestar = 0;
		}
	}
	dataDeArchivoEnRAM[tamanio] = '\0';
	* listaFinal = obtenerDataDeVariasPosiciones(dataDeArchivoEnRAM);

	if(tamanio % blockSize == 0 && tamanio != 0)
		pos++;

	free(path);
	return pos;
}

void modificarArchivos(t_list *listaFinal, char** listaDeBloques, uint32_t tamanio){
	uint32_t pos = 0, tamanioARestar, desplazamiento = 0;
	char textoAGuardar[tamanio];
	memset(textoAGuardar,0,tamanio);

	while(!list_is_empty(listaFinal)){
		pokemonDatoPosicion* valorActual = list_get(listaFinal, pos);
		char* nuevaLinea = generarNuevaLinea(valorActual);
		strcat(textoAGuardar, nuevaLinea);
		free(nuevaLinea);
		list_remove(listaFinal, 0);
		free(valorActual);
	}
	list_destroy(listaFinal);
	textoAGuardar[tamanio] = '\0';

	tamanioARestar = string_length(textoAGuardar);

	//strcat(textoAGuardar, "\0");

	char *path = agregarPuntoDeMontajeConMagicNumber("Blocks/");
	strcat(path, listaDeBloques[pos]);
	strcat(path, ".bin");

	while (tamanioARestar != 0) {

		FILE* archivo = fopen(path, "w");

		if(tamanioARestar > blockSize){
			fwrite(textoAGuardar + desplazamiento, blockSize, 1, archivo);
			fclose(archivo);

			pos++;
			free(path);
			path = agregarPuntoDeMontajeConMagicNumber("Blocks/");
			strcat(path, listaDeBloques[pos]);
			strcat(path, ".bin");
			//archivo = fopen(path, "w");

			desplazamiento += blockSize;

			tamanioARestar -= blockSize;
		}
		else{
			fwrite(textoAGuardar+desplazamiento, tamanioARestar, 1, archivo);
			fclose(archivo);


			tamanioARestar = 0;
		}
	}

	free(path);
}

void arrayStringToArrayConfig(uint32_t cantidadDeBlocks, char nuevoStringBloques[1000], pokemonMetadata* datosPokemon) {

	char** listaString = malloc(cantidadDeBlocks * 100);
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
	char* path = agregarPuntoDeMontajeConMagicNumber("/Files/");
	strcat(path, pokemon);


	pokemonMetadata* datosPokemon = malloc(sizeof(pokemonMetadata));
	if(access(path, F_OK) != -1){


		pokemonDatoPosicion *posicionPokemon = malloc(sizeof(pokemonDatoPosicion));
		posicionPokemon->posX=posX;
		posicionPokemon->posY=posY;
		posicionPokemon->cantidad=cantidad;

		strcat(path, "/Metadata.bin");

		char nuevoStringBloques[1000];
		t_config * configMetadata = validarArchivoAbierto(path, datosPokemon);

		t_list * lista;// = list_create();
		cantidadDeBlocks = obtenerDataFileSystem(datosPokemon->tamanio, datosPokemon->bloquesAsociados, &lista);

		aComparar = *posicionPokemon;

		pokemonDatoPosicion *valorEncontrado = (pokemonDatoPosicion*)list_find(lista, (void*)mismaPosicion);

		if(valorEncontrado != NULL) //Si encuentra el valor
		{
			uint32_t valorAnterior = valorEncontrado->cantidad;
			valorEncontrado->cantidad += posicionPokemon->cantidad;
			char* stringAnterior = string_itoa(valorAnterior);
			char* stringNuevo = string_itoa(valorEncontrado->cantidad);

			if(string_length(stringNuevo) > string_length(stringAnterior)){
				uint32_t tamanio = datosPokemon->tamanio;
				datosPokemon->tamanio += string_length(stringNuevo) - string_length(stringAnterior);
				if(datosPokemon->tamanio / blockSize > tamanio / blockSize)
				{
					char * puntoDeMontajeConMagic = agregarPuntoDeMontaje("");
					uint32_t nuevoBloque = obtenerBloqueLibre(blockCantBytes, puntoDeMontajeConMagic);
					free(puntoDeMontajeConMagic);

					cantidadDeBlocks++;
					char* str = malloc(10);
					snprintf(str, 10, "%d", nuevoBloque);
					datosPokemon->bloquesAsociados[cantidadDeBlocks] = str;

				}
			}

			free(stringAnterior);
			free(stringNuevo);
			free(posicionPokemon);
		}
		else{
			uint32_t tamanio = datosPokemon->tamanio;

			char* datoParaRellenar = generarNuevaLinea(posicionPokemon);
			datosPokemon->tamanio += string_length(datoParaRellenar);
			free(datoParaRellenar);

			list_add(lista, posicionPokemon);
			if(datosPokemon->tamanio / blockSize > tamanio / blockSize)
			{

				char * puntoDeMontajeConMagic = agregarPuntoDeMontaje("");
				uint32_t nuevoBloque = obtenerBloqueLibre(blockCantBytes, puntoDeMontajeConMagic);
				free(puntoDeMontajeConMagic);

				cantidadDeBlocks++;
				char* str = malloc(10);
				snprintf(str, 10, "%d", nuevoBloque);
				datosPokemon->bloquesAsociados[cantidadDeBlocks] = str;

			}
		}
		modificarArchivos(lista, datosPokemon->bloquesAsociados, datosPokemon->tamanio);


		cantidadDeBlocks++; //se le suma una para que las iteraciones se hagas correctamente

		arrayStringToArrayConfig(cantidadDeBlocks, nuevoStringBloques, datosPokemon);


		config_set_value(configMetadata, "OPEN", "Y");
		char* nuevoSize = string_itoa(datosPokemon->tamanio);
		config_set_value(configMetadata, "SIZE", nuevoSize);
		config_set_value(configMetadata, "BLOCKS", nuevoStringBloques);
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


		char * puntoDeMontajeConMagic = agregarPuntoDeMontaje("");
		int bloque = obtenerBloqueLibre(blockCantBytes, puntoDeMontajeConMagic);
		free(puntoDeMontajeConMagic);


		char str[12];
		char* copiar = string_itoa(bloque);
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
	return;
}



uint32_t administrarCatchPokemon(char* pokemon, uint32_t posX, uint32_t posY){
	t_log* logger = iniciar_logger("GAMECARD.log", "Catch");
	uint32_t cantidadDeBlocks;
	char* path = agregarPuntoDeMontajeConMagicNumber("/Files/");
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
			cantidadDeBlocks = obtenerDataFileSystem(datosPokemon->tamanio, datosPokemon->bloquesAsociados, &lista);

			aComparar = *posicionPokemon;

			pokemonDatoPosicion *valorEncontrado = (pokemonDatoPosicion *)list_find(lista, (void*)mismaPosicion);
			if(valorEncontrado != NULL){ //Si encuentra el valor

				char nuevoStringBloques[1000];
				valorEncontrado->cantidad--;

				if(valorEncontrado->cantidad == 0){
					uint32_t tamanioPrevio = datosPokemon->tamanio;

					char* datoTemporal = generarNuevaLinea(posicionPokemon);
					datosPokemon->tamanio -= string_length(datoTemporal);
					free(datoTemporal);

					if(datosPokemon->tamanio / blockSize < tamanioPrevio / blockSize){
						uint32_t colaAVaciar = atoi(datosPokemon->bloquesAsociados[cantidadDeBlocks]);

						char * puntoDeMontajeConMagic = agregarPuntoDeMontaje("");
						limpiarBloque(colaAVaciar, blockCantBytes, puntoDeMontajeConMagic);
						free(puntoDeMontajeConMagic);

						cantidadDeBlocks--; //se hace esto para no incluir el último valor del char**
					}

					list_remove_and_destroy_by_condition(lista, (void*)mismaPosicion, (void*)free);

					cantidadDeBlocks++; //se le suma una para que las iteraciones se hagas correctamente

					arrayStringToArrayConfig(cantidadDeBlocks, nuevoStringBloques, datosPokemon);


					config_set_value(configMetadata, "BLOCKS", nuevoStringBloques);

					char* sizeString = string_itoa(datosPokemon->tamanio);
					config_set_value(configMetadata, "SIZE", sizeString);
					free(sizeString);
				}
				else if(valorEncontrado->cantidad / blockSize < (valorEncontrado->cantidad+1) / blockSize){
					uint32_t colaAVaciar = atoi(datosPokemon->bloquesAsociados[cantidadDeBlocks]);

					char * puntoDeMontajeConMagic = agregarPuntoDeMontaje("");
					limpiarBloque(colaAVaciar, blockCantBytes, puntoDeMontajeConMagic);
					free(puntoDeMontajeConMagic);

					cantidadDeBlocks--;
				}

				if(!list_is_empty(lista))
					modificarArchivos(lista, datosPokemon->bloquesAsociados, datosPokemon->tamanio);
				else
					list_destroy(lista);


				config_set_value(configMetadata, "OPEN", "Y");
				config_save(configMetadata);

				config_destroy(configMetadata);

				cantidadDeBlocks++;
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
	datosDePosicionPokemon->cantidadDeParesDePosiciones = 0; //para el caso que no encuentre
	datosDePosicionPokemon->posiciones = list_create();

	uint32_t cantidadDeBlocks;
	char* path = agregarPuntoDeMontajeConMagicNumber("/Files/");
	strcat(path, pokemon);

	char metadataString[] = "/Metadata.bin";
	strcat(path, metadataString);


	pokemonMetadata* datosPokemon = malloc(sizeof(pokemonMetadata));
	if(access(path, F_OK) != -1){
		t_config * configMetadata = validarArchivoAbierto(path, datosPokemon);

		t_list * lista;// = list_create();
		cantidadDeBlocks = obtenerDataFileSystem(datosPokemon->tamanio, datosPokemon->bloquesAsociados, &lista);

		datosDePosicionPokemon->cantidadDeParesDePosiciones = (uint32_t)list_size(lista);



		t_list* listaAuxiliar;
		uint32_t offset = 0;

		while(!list_is_empty(lista)){
			listaAuxiliar = list_take_and_remove(lista, 1);
			pokemonDatoPosicion* structADeserializar;
			structADeserializar = list_get(listaAuxiliar, 0);


			printf("X de struct %d\n",structADeserializar->posX);
			printf("Y de struct %d\n",structADeserializar->posY);
			list_add(datosDePosicionPokemon->posiciones, &(structADeserializar->posX));
			list_add(datosDePosicionPokemon->posiciones, &(structADeserializar->posY));


			list_destroy(listaAuxiliar);
		}

		list_destroy_and_destroy_elements(lista, (void*)free);

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
