#include "dynamicCache.h"
#include "../brokerColas.h"

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
	char* oldestDate = tablaElementos[0].fechaCreacion;

	for(int i = 1; i < tamanioTabla; i++){
		ItemTablaDinamica current = tablaElementos[i];

		if(!current.estaVacio && current.fechaCreacion != NULL && (oldestDate == NULL || esTiempoMasAntiguo(current.fechaCreacion, oldestDate))){
			oldestDate = current.fechaCreacion;
			oldestposicion = i;
		}
	}

	return tablaElementos[oldestposicion].ID;
}

int calcularLRU(){
	int oldestposicion = 0;
	char* oldestDate = NULL;

	if(tamanioTabla > 1){
		for(int i = 1; i < tamanioTabla; i++){
			ItemTablaDinamica current = tablaElementos[i];

			if(current.ID != -1 && (oldestDate == NULL || (!current.estaVacio && esTiempoMasAntiguo(current.fechaUltimoUso, oldestDate)))){
				oldestDate = current.fechaCreacion;
				oldestposicion = i;
			}
		}
	}

	return tablaElementos[oldestposicion].ID;
}

int eliminarVictima(TipoCola cola){
	long itemAEliminar;

	if(strcmp(algoritmoEleccionDeVictima, "FIFO") == 0){
		itemAEliminar = calcularFIFO();
	} else{
		itemAEliminar = calcularLRU();
	}

	if(itemAEliminar == -1) return 0;

	eliminarItem(itemAEliminar);
	//log_info(loggerObligatorio, "Me FUI DE ELIMINARITEM");
	//eliminarMensajeDeLista(itemAEliminar, cola);
	//log_info(loggerObligatorio, "Me FUI DE ELIMINARMENSAJEDELISTA");

	obtenerDump();

	return 1;
}

int hayEspacio(int espacioRequerido, int *posicion){
	//log_info(loggerInterno, "Espacio requerido: %d", espacioRequerido);

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

int obtenerDesplazamientoMinimo(int espacioRequerido){
	return espacioRequerido < tamanioParticionMinima ? tamanioParticionMinima : espacioRequerido;
}

void modificarTablaVacio(ItemTablaDinamica *tabla, int espacioRequerido, int posicionVector){
	int desplazamiento = obtenerDesplazamientoMinimo(espacioRequerido);
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
		(*tabla)[0].fechaCreacion = NULL;
		(*tabla)[0].fechaUltimoUso = NULL;
	}

	for(int i = estaVacio; i < tamanioTabla; i++){
		(*tabla)[i].ID = -1;
		(*tabla)[i].posicion = 0;
		(*tabla)[i].tamanio = 0;
		(*tabla)[i].estaVacio = 1;
		(*tabla)[i].fechaCreacion = NULL;
		(*tabla)[i].fechaUltimoUso = NULL;
	}
}

void inicializarDataBasica(t_config* config, t_log* loggerObligatorioAsignar, t_log* loggerInternoAsignar) {
	//semCacheTabla = malloc(sizeof(sem_t));
	//sem_init(semCacheTabla, 0, 1);

	tamanioCache = (int)config_get_int_value(config, "TAMANO_MEMORIA");
	tamanioParticionMinima = (int)config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
	algoritmoEleccionDeParticionLibre = config_get_string_value(config,"ALGORITMO_PARTICION_LIBRE");
	algoritmoEleccionDeVictima = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
	frecuenciaCompactacion = (int)config_get_int_value(config, "FRECUENCIA_COMPACTACION");
	tamanioTabla = tamanioCache / tamanioParticionMinima;
	loggerObligatorio = loggerObligatorioAsignar;
	loggerInterno = loggerInternoAsignar;

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
	obtenerDump();
	//log_info(loggerObligatorio, "agregarItem: antes wait");
	//sem_wait(semCacheTabla);
	//log_info(loggerObligatorio, "agregarItem: después wait");

	int posicionVacioAModificar;
	int hayEspacioParaItem = hayEspacio(tamanioItem, &posicionVacioAModificar);
	int logrado = 1;

	while(!hayEspacioParaItem){
		logrado = eliminarVictima(cola);

		if(!logrado) break;

		hayEspacioParaItem = hayEspacio(tamanioItem, &posicionVacioAModificar);
	}

	if(logrado)
	{
		//log_info(loggerObligatorio, "ANTES AGREGARELEMENTOVALIDO");
		agregarElementoValido(posicionVacioAModificar, tamanioItem, item, ID, IDCorrelativo, cola);
		//log_info(loggerObligatorio, "DESPUÉS AGREGARELEMENTOVALIDO");
		obtenerDump();
	} else {
		//log_info(loggerInterno, "No se encontró víctima");
	}

	//log_info(loggerObligatorio, "agregarItem: antes post");
	//sem_post(semCacheTabla);
	//log_info(loggerObligatorio, "agregarItem: después post");
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

void* obtenerItem(long ID)
{
	//log_info(loggerObligatorio, "obtenerItem: antes wait");
	//sem_wait(semCacheTabla);
	//log_info(loggerObligatorio, "obtenerItem: después wait");

	ItemTablaDinamica* item = obtenerItemTablaDinamica(ID);

	if(item == NULL) {
		//sem_post(semCacheTabla);
		return NULL;
	}

	void* valorDeMierda = obtenerValor(item->tamanio, item->posicion);

	//log_info(loggerObligatorio, "obtenerItem: antes post");
	//sem_post(semCacheTabla);
	//log_info(loggerObligatorio, "obtenerItem: después post");

	return valorDeMierda;
}

void cambiarLRU(long ID)
{
	ItemTablaDinamica* item = obtenerItemTablaDinamica(ID);

	if(item != NULL){
		free(item->fechaUltimoUso);
		item->fechaUltimoUso = temporal_get_string_time();
	}
}

void consolidarCache(int posicionElementoVacio, int posicionElemento){
	ItemTablaDinamica* elementoVacio = &(tablaVacios[posicionElementoVacio]);
	int posicionBuscada = elementoVacio->posicion + elementoVacio->tamanio;
	int i = 0;

	while(i < tamanioTabla){
		//log_info(loggerObligatorio, "HOLA");
		for(i = 0; i < tamanioTabla; i++){
			//log_info(loggerObligatorio, "KOMO ESTAS");
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

			if(!consolidar) continue; //FORROOOOOOOOOOOOOOOOOOOO

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

void compactarCache(){
	obtenerDump();
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
			tablaCompactada[posicionTablaNueva].cola = elementoActual.cola;
			posicionVieja = elementoActual.posicion;
			tablaCompactada[posicionTablaNueva].posicion = posicionNueva;
			tablaCompactada[posicionTablaNueva].estaVacio = 0;
			moverBloque(elementoActual.tamanio, posicionVieja, posicionNueva);
			posicionNueva += obtenerDesplazamientoMinimo(elementoActual.tamanio);
			posicionTablaNueva++;
		}
	}

	modificarTablaVacio(tablaVacios, posicionNueva, 0);
	memcpy(tablaElementos, tablaCompactada, sizeof(ItemTablaDinamica) * tamanioTabla);

	//Log obligatorio.
	log_info(loggerObligatorio, "Ejecución de compactación.");

	particionesLiberadas = 0;
}

void eliminarItem(long ID){
	log_info(loggerObligatorio, "Entro a eliminarItem gg");
	particionesLiberadas++;
	int posDatoAEliminar = obtenerPosicionPorID(ID);
	int posNuevoVacio = obtenerPrimeraParticion(tablaVacios, 1);
	int espacioVacio = obtenerDesplazamientoMinimo(tablaElementos[posDatoAEliminar].tamanio);

	tablaVacios[posNuevoVacio].ID = posNuevoVacio;
	tablaVacios[posNuevoVacio].posicion = tablaElementos[posDatoAEliminar].posicion;
	tablaVacios[posNuevoVacio].estaVacio = 0;
	tablaVacios[posNuevoVacio].tamanio = espacioVacio;

	tablaElementos[posDatoAEliminar].ID = -1;
	tablaElementos[posDatoAEliminar].posicion = 0;
	tablaElementos[posDatoAEliminar].estaVacio = 1;
	tablaElementos[posDatoAEliminar].tamanio = 0;

	if(tablaElementos[posDatoAEliminar].fechaCreacion != NULL)
	{
		free(tablaElementos[posDatoAEliminar].fechaCreacion);
		tablaElementos[posDatoAEliminar].fechaCreacion = NULL;
	}

	if(tablaElementos[posDatoAEliminar].fechaUltimoUso != NULL)
	{
		free(tablaElementos[posDatoAEliminar].fechaUltimoUso);
		tablaElementos[posDatoAEliminar].fechaUltimoUso = NULL;
	}

	//Log obligatorio.
	log_info(loggerObligatorio, "Eliminada partición con posición de inicio %d.", posDatoAEliminar);

	consolidarCache(posNuevoVacio, posDatoAEliminar);

	//log_info(loggerObligatorio, "UNO");

	obtenerDump();

	if(frecuenciaCompactacion <= 1 || particionesLiberadas == frecuenciaCompactacion){
		//log_info(loggerObligatorio, "DOS");
		compactarCache();
		//log_info(loggerObligatorio, "TRES");
		obtenerDump();
	}
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
				"Partición %-5d 0x%-3X - 0x%-10X\t[X]\tsize %-5db\tLRU: %-15s\tCOLA: %-5s\tID: %ld\n",
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

bool ordenamientoDump(void* primerDatoVoid, void* segundoDatoVoid){
	ItemTablaDinamica* primerDato = (ItemTablaDinamica*)primerDatoVoid;
	ItemTablaDinamica* segundoDato = (ItemTablaDinamica*)segundoDatoVoid;

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

void agregarSuscriptorEnviado(long IDMensaje, Suscriptor** suscriptor){
	for(int i = 0; i < tamanioTabla; i++){
		if(tablaElementos[i].ID == IDMensaje){
			int yaEnviado = list_contains_int(tablaElementos[i].suscriptoresEnviados, (*suscriptor)->modulo);

			if(!yaEnviado){
				list_add(tablaElementos[i].suscriptoresEnviados, &((*suscriptor)->modulo));
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

t_list* obtenerSuscriptoresEnviados(long IDMensaje){
	t_list* suscriptoresEnviados = NULL;

	for(int i = 0; i < tamanioTabla; i++){
		if(tablaElementos[i].ID == IDMensaje){
			suscriptoresEnviados = tablaElementos[i].suscriptoresEnviados;
		}
	}

	return suscriptoresEnviados;
}

int esSuscriptorEnviado(t_list* suscriptoresEnviados, Suscriptor suscriptor){

	for(int i = 0; i < list_size(suscriptoresEnviados); i++){
		TipoModulo* suscriptorRecibido = list_get(suscriptoresEnviados, i);

		if(*suscriptorRecibido == suscriptor.modulo){
			return 1;
		}
	}

	return 0;
}

void liberarCache(){
	for(int i = 0; i < tamanioTabla; i++){
		if(!tablaElementos[i].estaVacio)
		{
			free(tablaElementos[i].suscriptoresEnviados);
			free(tablaElementos[i].suscriptoresRecibidos);
			free(tablaElementos[i].fechaCreacion);
			free(tablaElementos[i].fechaUltimoUso);
		}
	}

	free(tablaElementos);
	free(tablaVacios);

	liberarBasicCache();
}

