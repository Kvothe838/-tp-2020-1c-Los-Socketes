#include "dynamicCache.h"

sem_t mutexCache;
int particionesLiberadas = 0;

int esTiempoMasAntiguo(char* masAntiguo, char* masNuevo){
	return strcmp(masAntiguo, masNuevo) < 0;
}

int calcularBestFit(int desiredSize) {
	int found = -1;
	int bestposicion, *bestDifference = NULL;

	for(int i = 0; i < tamanioTabla; i++){
		ItemTablaDinamica currentEntry = tablaVacios[i];

		if(!currentEntry.estaVacio && currentEntry.tamanio >= desiredSize){
			int diff = currentEntry.tamanio - desiredSize;
			if(bestDifference >= 0 && (bestDifference == NULL || diff < *bestDifference)){
				*bestDifference = diff;
				bestposicion = i;
				found = 1;
			}
		}
	}

	return found >= 0 ? bestposicion : found;
}

int calcularFIFO(){
	int oldestposicion = 0;
	char* oldestDate = "";

	for(int i = 0; i < tamanioTabla; i++){
		ItemTablaDinamica current = tablaElementos[i];

		if(!current.estaVacio && esTiempoMasAntiguo(oldestDate, current.fechaCreacion)){
			oldestDate = current.fechaCreacion;
			oldestposicion = i;
		}
	}

	return tablaElementos[oldestposicion].ID;
}

int calcularLRU(){
	int oldestposicion = 0;
	char* oldestDate = tablaElementos[0].fechaCreacion;

	if(tamanioTabla > 1){
		for(int i = 1; i < tamanioTabla; i++){
			ItemTablaDinamica current = tablaElementos[i];

			if(!current.estaVacio && esTiempoMasAntiguo(oldestDate, current.fechaUltimoUso)){
				oldestDate = current.fechaCreacion;
				oldestposicion = i;
			}
		}
	}

	return tablaElementos[oldestposicion].ID;
}

void eliminarVictima(){
	if(strcmp(algoritmoEleccionDeVictima, "FIFO") == 0){
		eliminarItem(calcularFIFO());
	} else{
		eliminarItem(calcularLRU());
	}
}

int hayEspacio(int espacioRequerido, int *posicion){
	if(strcmp(algoritmoEleccionDeParticionLibre, "FF") == 0){
		for(*posicion = 0; *posicion < tamanioTabla; (*posicion)++){
			if(!tablaVacios[*posicion].estaVacio && tablaVacios[*posicion].tamanio >= espacioRequerido)
				return 1;
		}
	} else {
		int mejorPosicion = calcularBestFit(espacioRequerido);

		if(mejorPosicion >= 0){
			*posicion = mejorPosicion;
			return 1;
		}
	}

	return 0;
}

int obtenerPrimeraParticion(ItemTablaDinamica *tabla, int estaVacio){
	for(int i = 0; i < tamanioTabla; i++){
		if(tabla[i].estaVacio == estaVacio)
			return i;
	}

	return -1;
}

int obtenerDesplazamientoMinimo(int tamanioParticionMinima, int espacioRequerido){
	return espacioRequerido < tamanioParticionMinima ? tamanioParticionMinima : espacioRequerido;
}

void modificarTablaVacio(ItemTablaDinamica *tabla, int espacioRequerido, int posicionVector){
	int desplazamiento = obtenerDesplazamientoMinimo(tamanioParticionMinima, espacioRequerido);
	if((tabla[posicionVector].tamanio -= desplazamiento) <= 0){
		tabla[posicionVector].posicion = 0;
		tabla[posicionVector].ID = -1;
		tabla[posicionVector].estaVacio = 1;
		tabla[posicionVector].tamanio = 0;
		/*
		 * si llega acá es porque el hayEspacio dio true, entonces puede ocurrir el caso
		 * que haya un lugar de espacio que el tamanio de negativo
		 *
		*/
	} else {
		tabla[posicionVector].posicion += desplazamiento;
	}
}

int obtenerPosicionPorID(long ID){
	for(int i = 0; i < tamanioTabla; i++){
		if(tablaElementos[i].ID == ID)
			return i;
	}

	return -1;
}

void inicializarTabla(ItemTablaDinamica **tabla, int estaVacio){
	*tabla = (ItemTablaDinamica*)malloc(tamanioTabla * sizeof(ItemTablaDinamica));

	if(estaVacio){
		(*tabla)[0].ID = 0;
		(*tabla)[0].posicion = 0;
		(*tabla)[0].tamanio = tamanioCache;
		(*tabla)[0].estaVacio = 0;
	}

	for(int i = estaVacio; i < tamanioTabla; i++){
		(*tabla)[i].ID = -1;
		(*tabla)[i].posicion = 0;
		(*tabla)[i].tamanio = 0;
		(*tabla)[i].estaVacio = 1;
	}
}

void inicializarDataBasica(t_config* config, t_log* loggerParaAsignar) {
	sem_init(&mutexCache, 0, 1);

	tamanioCache = (int)config_get_int_value(config, "TAMANO_MEMORIA");
	tamanioParticionMinima = (int)config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
	algoritmoEleccionDeParticionLibre = config_get_string_value(config,"ALGORITMO_PARTICION_LIBRE");
	algoritmoEleccionDeVictima = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
	frecuenciaCompactacion = (int)config_get_string_value(config, "FRECUENCIA_COMPACTACION");
	tamanioTabla = tamanioCache / tamanioParticionMinima;
	logger = loggerParaAsignar;

	inicializarTabla(&tablaElementos, 0);
	inicializarTabla(&tablaVacios, 1);
	inicializarCache(tamanioCache);
}

void agregarElementoValido(int posicionVacioAModificar, int tamanioItem, void* item, long ID, long IDCorrelativo,
		TipoCola cola) {
	int posicionElementoAModificar = obtenerPrimeraParticion(tablaElementos, 1);
	tablaElementos[posicionElementoAModificar].posicion = tablaVacios[posicionVacioAModificar].posicion;
	tablaElementos[posicionElementoAModificar].tamanio = tamanioItem;
	tablaElementos[posicionElementoAModificar].estaVacio = 0;
	tablaElementos[posicionElementoAModificar].fechaCreacion = temporal_get_string_time();
	tablaElementos[posicionElementoAModificar].fechaUltimoUso = temporal_get_string_time();
	tablaElementos[posicionElementoAModificar].ID = ID;
	tablaElementos[posicionElementoAModificar].IDCorrelativo = IDCorrelativo;
	tablaElementos[posicionElementoAModificar].cola = cola;
	tablaElementos[posicionElementoAModificar].suscriptoresRecibidos = list_create();
	tablaElementos[posicionElementoAModificar].suscriptoresEnviados = list_create();
	modificarTablaVacio(tablaVacios, tamanioItem, posicionVacioAModificar);
	guardarValor(item, tamanioItem,	tablaElementos[posicionElementoAModificar].posicion);
}

void agregarItem(void* item, int tamanioItem, long ID, long IDCorrelativo, TipoCola cola){
	sem_wait(&mutexCache);

	int posicionVacioAModificar;
	int hayEspacioParaItem = hayEspacio(tamanioItem, &posicionVacioAModificar);

	while(!hayEspacioParaItem){
		eliminarVictima();
		hayEspacioParaItem = hayEspacio(tamanioItem, &posicionVacioAModificar);
	}

	agregarElementoValido(posicionVacioAModificar, tamanioItem, item, ID, IDCorrelativo, cola);

	sem_post(&mutexCache);
}

ItemTablaDinamica* obtenerItemTablaDinamica(long ID){
	for(int i = 0; i < tamanioTabla; i++){
		if(tablaElementos[i].ID == ID){
			return &tablaElementos[i];
		}
	}

	return NULL;
}

int* obtenerTamanioItem(long ID){
	ItemTablaDinamica* item = obtenerItemTablaDinamica(ID);

	if(item == NULL) return NULL;

	return &(item->tamanio);
}

long* obtenerIDCorrelativoItem(long ID)
{
	ItemTablaDinamica* item = obtenerItemTablaDinamica(ID);

	if(item == NULL) return NULL;

	return &(item->IDCorrelativo);
}

void* obtenerItem(long ID){
	ItemTablaDinamica* item = obtenerItemTablaDinamica(ID);

	if(item == NULL) return NULL;

	free(item->fechaUltimoUso);
	item->fechaUltimoUso = temporal_get_string_time();

	return obtenerValor(item->tamanio, item->posicion);
}

void consolidarCache(ItemTablaDinamica* elementoVacio, int posicionElementoVacio, int posicionElemento){
	int posicionBuscada = elementoVacio->posicion + elementoVacio->tamanio;
	int i = 0;

	while(i < tamanioTabla){
		for(i = 0; i < tamanioTabla; i++){
			if(i == posicionElementoVacio) continue;

			ItemTablaDinamica elementoActual = tablaVacios[i];
			int posicionConsolidada, posicionAEliminar, posicionAModificar, consolidar = 1;

			if(elementoActual.posicion == posicionBuscada){
				posicionConsolidada = elementoVacio->posicion;
			} else if(elementoVacio->posicion != 0 && elementoActual.posicion + elementoActual.tamanio == elementoVacio->posicion){
				posicionConsolidada = elementoActual.posicion;
			} else {
				consolidar = 0;
			}

			if(consolidar){
				posicionAEliminar = i > posicionElementoVacio ? i : posicionElementoVacio;
				posicionAModificar = i < posicionElementoVacio ? i : posicionElementoVacio;

				tablaVacios[posicionAModificar].tamanio = elementoVacio->tamanio + tablaVacios[i].tamanio;
				tablaVacios[posicionAModificar].posicion = posicionConsolidada;
				tablaVacios[posicionAEliminar].estaVacio = 1;
				tablaVacios[posicionAEliminar].ID = -1;
				tablaVacios[posicionAEliminar].tamanio = 0;
				tablaVacios[posicionAEliminar].posicion = 0;

				posicionElementoVacio = posicionAModificar;
				elementoVacio = &(tablaVacios[posicionAModificar]);

				break;
			}
		}
	}
}

void compactarCache(){
	ItemTablaDinamica *tablaCompactada = NULL;
	int posicionNueva = 0, posicionVieja, posicionTablaNueva = 0;

	inicializarTabla(&tablaCompactada, 0);
	inicializarTabla(&tablaVacios, 1);

	for(int i = 0; i < tamanioTabla; i++)
	{
		ItemTablaDinamica elementoActual = tablaElementos[i];

		if(!elementoActual.estaVacio)
		{
			tablaCompactada[posicionTablaNueva].ID = elementoActual.ID;
			tablaCompactada[posicionTablaNueva].IDCorrelativo = elementoActual.IDCorrelativo;
			tablaCompactada[posicionTablaNueva].suscriptoresEnviados = elementoActual.suscriptoresEnviados;
			tablaCompactada[posicionTablaNueva].suscriptoresRecibidos = elementoActual.suscriptoresRecibidos;
			tablaCompactada[posicionTablaNueva].tamanio = elementoActual.tamanio;
			tablaCompactada[posicionTablaNueva].fechaCreacion = elementoActual.fechaCreacion;
			tablaCompactada[posicionTablaNueva].fechaUltimoUso = elementoActual.fechaUltimoUso;
			posicionVieja = elementoActual.posicion;
			tablaCompactada[posicionTablaNueva].posicion = posicionNueva;
			tablaCompactada[posicionTablaNueva].estaVacio = 0;
			moverBloque(elementoActual.tamanio, posicionVieja, posicionNueva);
			posicionNueva += obtenerDesplazamientoMinimo(tamanioParticionMinima, elementoActual.tamanio);
			posicionTablaNueva++;
		}
	}

	modificarTablaVacio(tablaVacios, posicionNueva, 0);
	memcpy(tablaElementos, tablaCompactada, sizeof(ItemTablaDinamica) * tamanioTabla);

	//Log obligatorio.
	log_info(logger, "Ejecución de compactación.");

	particionesLiberadas = 0;
}

void eliminarItem(long ID){
	sem_wait(&mutexCache);

	particionesLiberadas++;
	int posDatoAEliminar = obtenerPosicionPorID(ID);
	int posNuevoVacio = obtenerPrimeraParticion(tablaVacios, 1);
	int espacioVacio = obtenerDesplazamientoMinimo(tamanioParticionMinima, tablaElementos[posDatoAEliminar].tamanio);

	tablaVacios[posNuevoVacio].ID = posNuevoVacio;
	tablaVacios[posNuevoVacio].posicion = tablaElementos[posDatoAEliminar].posicion;
	tablaVacios[posNuevoVacio].estaVacio = 0;
	tablaVacios[posNuevoVacio].tamanio = espacioVacio;

	tablaElementos[posDatoAEliminar].ID = -1;
	tablaElementos[posDatoAEliminar].posicion = 0;
	tablaElementos[posDatoAEliminar].estaVacio = 1;
	tablaElementos[posDatoAEliminar].tamanio = 0;

	//Log obligatorio.
	log_info(logger, "Eliminada partición con posición de inicio %d.", posDatoAEliminar);

	consolidarCache(&(tablaVacios[posNuevoVacio]), posNuevoVacio, posDatoAEliminar);

	if(frecuenciaCompactacion <= 1 || particionesLiberadas == frecuenciaCompactacion){
		compactarCache();
	}

	sem_post(&mutexCache);
}

//DUMP DE CACHE (COMIENZO)
void imprimirDatos(t_list* listaDeParticiones){
	uint32_t posicion = 0;
	char stringFinal[1000];
	FILE* archivoDump = fopen("dump.txt", "w+");

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	sprintf(stringFinal, "Dump: %d/%d/%d - %s\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, temporal_get_string_time());
	fwrite(stringFinal, string_length(stringFinal), 1, archivoDump);



	while(posicion < list_size(listaDeParticiones)){
		ItemTablaDinamica *dato = list_get(listaDeParticiones, posicion);


		if(dato->fechaUltimoUso != NULL){
			sprintf(stringFinal,
				"Partición %-5d 0x%-3X - 0x%-10X\t[X]\tsize %-5d\tLRU: %-15s\tCOLA: %-5s\tID: %d\n",
				(posicion+1),
				dato->posicion,
				(dato->posicion + dato->tamanio) - 1,
				(dato->tamanio),
				dato->fechaUltimoUso,
				(char*)tipoColaToString(dato->cola),
				dato->ID);
		}
		else{
			sprintf(stringFinal,
				"Partición %-5d 0x%-3X - 0x%-10X\t[L]\tsize %d\n",
				(posicion+1),
				dato->posicion,
				(dato->posicion + dato->tamanio) - 1,
				(dato->tamanio)
				);
		}

		fwrite(stringFinal, string_length(stringFinal), 1, archivoDump);
		posicion++;
	}
	fclose(archivoDump);

}

bool ordenamientoDump(ItemTablaDinamica *primerDato, ItemTablaDinamica *segundoDato){
	return primerDato->posicion < segundoDato->posicion;
}

void obtenerDump(){
	t_list* lista = list_create();
	for(int i = 0; i < tamanioTabla; i++){
		if(!tablaElementos[i].estaVacio){
			list_add(lista, &(tablaElementos[i]));
		}

		if(!tablaVacios[i].estaVacio){
			list_add(lista, &(tablaVacios[i]));
		}
	}
	list_sort(lista, ordenamientoDump);
	imprimirDatos(lista);
	list_destroy(lista);
}
//DUMP DE CACHE (FIN)

//SUSCRIPTORES

void* mapearSuscriptorAModulo(void* suscriptor){
	Suscriptor* suscriptorCasteado = (Suscriptor*)suscriptor;

	return &(suscriptorCasteado->modulo);
}

void agregarSuscriptorEnviado(long IDMensaje, Suscriptor* suscriptor){
	for(int i = 0; i < tamanioTabla; i++){
		if(tablaElementos[i].ID == IDMensaje){
			t_list* moduloSuscriptores = list_map(tablaElementos[i].suscriptoresEnviados, &mapearSuscriptorAModulo);
			int yaEnviado = list_contains_int(moduloSuscriptores, suscriptor->modulo);

			if(!yaEnviado){
				list_add(tablaElementos[i].suscriptoresRecibidos, &suscriptor);
			}
		}
	}
}

void agregarSuscriptorRecibido(long IDMensaje, Suscriptor* suscriptor){
	for(int i = 0; i < tamanioTabla; i++){
		if(tablaElementos[i].ID == IDMensaje){
			list_add(tablaElementos[i].suscriptoresRecibidos, &(suscriptor->modulo));
		}
	}
}

t_list* obtenerSuscriptoresRecibidos(long IDMensaje){
	t_list* suscriptoresRecibidos = NULL;

	for(int i = 0; i < tamanioTabla; i++){
		if(tablaElementos[i].ID == IDMensaje){
			suscriptoresRecibidos = tablaElementos[i].suscriptoresRecibidos;
		}
	}

	return suscriptoresRecibidos;
}

int esSuscriptorRecibido(t_list* suscriptoresRecibidos, Suscriptor suscriptor){
	//t_list* moduloSuscriptores = list_map(suscriptoresRecibidos, &mapearSuscriptorAModulo);

	for(int i = 0; i < list_size(suscriptoresRecibidos); i++){
		TipoModulo* suscriptorRecibido = list_get(suscriptoresRecibidos, i);

		if(*suscriptorRecibido == suscriptor.modulo){
			return 1;
		}
	}

	return 0;

	//return list_contains_int(moduloSuscriptores, suscriptor.modulo);
}

/*void modificarSuscriptor(Suscriptor nuevoSuscriptor){
	_Bool buscarSuscriptor(void* suscriptorGeneric){
			Suscriptor* suscriptor = (Suscriptor*)suscriptorGeneric;

			return suscriptor->modulo == nuevoSuscriptor.modulo;
	}

	for(int i = 0; i < tamanioTabla; i++){
		Suscriptor* suscriptorEncontrado = list_find(tablaElementos[i].suscriptoresRecibidos, &buscarSuscriptor);

		if(suscriptorEncontrado != NULL){
			suscriptorEncontrado->socket = nuevoSuscriptor.socket;
		}
	}
}*/



