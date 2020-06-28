#include "dynamicCache.h"

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

void modificarTablaVacio(ItemTablaDinamica *tabla, int espacioRequerido, int posicionVector,
		int tamanioParticionMinima){
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
	}
	else{
		tabla[posicionVector].posicion += desplazamiento;
	}
}

int obtenerPosicionPorID(int ID){
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

void agregarElementoValido(int posicionVacioAModificar, int tamanioItem, void* item, long ID, long* IDCorrelativo,
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
	modificarTablaVacio(tablaVacios, tamanioItem, posicionVacioAModificar, tamanioParticionMinima);
	guardarValor(item, tamanioItem,	tablaElementos[posicionElementoAModificar].posicion);
}

void agregarItem(void* item, int tamanioItem, long ID, long* IDCorrelativo, TipoCola cola){
	int posicionVacioAModificar;
	int hayEspacioParaItem = hayEspacio(tamanioItem, &posicionVacioAModificar);

	while(!hayEspacioParaItem){
		eliminarVictima();
		hayEspacioParaItem = hayEspacio(tamanioItem, &posicionVacioAModificar);
	}

	agregarElementoValido(posicionVacioAModificar, tamanioItem, item, ID, IDCorrelativo, cola);
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

	return &item->tamanio;
}

void* obtenerItem(long ID){
	ItemTablaDinamica* item = obtenerItemTablaDinamica(ID);

	if(item == NULL) return NULL;

	free(item->fechaUltimoUso);
	item->fechaUltimoUso = temporal_get_string_time();

	return obtenerValor(item->tamanio, item->posicion);
}

void consolidarCache(){

}

void compactarCache(){
	ItemTablaDinamica *tablaCompactada = NULL, *tablaNuevoVacio = NULL;
	int posicionNueva = 0, posicionVieja;

	inicializarTabla(&tablaCompactada, 0);
	inicializarTabla(&tablaNuevoVacio, 1);

	for(int i = 0; i < tamanioTabla; i++)
	{
		ItemTablaDinamica elementoActual = tablaElementos[i];

		if(!elementoActual.estaVacio)
		{
			tablaCompactada[i].ID = elementoActual.ID;
			tablaCompactada[i].IDCorrelativo = elementoActual.IDCorrelativo;
			tablaCompactada[i].suscriptoresEnviados = elementoActual.suscriptoresEnviados;
			tablaCompactada[i].suscriptoresRecibidos = elementoActual.suscriptoresRecibidos;
			tablaCompactada[i].tamanio = elementoActual.tamanio;
			tablaCompactada[i].fechaCreacion = elementoActual.fechaCreacion;
			tablaCompactada[i].fechaUltimoUso = elementoActual.fechaUltimoUso;
			posicionVieja = elementoActual.posicion;
			tablaCompactada[i].posicion = posicionNueva;
			tablaCompactada[i].estaVacio = 0;
			moverBloque(elementoActual.tamanio, posicionVieja, posicionNueva);
			posicionNueva += obtenerDesplazamientoMinimo(tamanioParticionMinima, elementoActual.tamanio);
		}
	}

	modificarTablaVacio(tablaNuevoVacio, posicionNueva, 0, tamanioParticionMinima);
	memcpy(tablaElementos, tablaCompactada, sizeof(ItemTablaDinamica) * tamanioTabla);
	memcpy(tablaVacios, tablaNuevoVacio, sizeof(ItemTablaDinamica) * tamanioTabla);

	//Log obligatorio.
	log_info(logger, "Ejecución de compactación.");

	particionesLiberadas = 0;
}

void eliminarItem(long ID){
	particionesLiberadas++;
	int posDatoAEliminar = obtenerPosicionPorID(ID);
	int posNuevoVacio = obtenerPrimeraParticion(tablaVacios, 1);
	int espacioVacio = obtenerDesplazamientoMinimo(tamanioParticionMinima, tablaElementos[posDatoAEliminar].tamanio);

	tablaVacios[posNuevoVacio].ID = posNuevoVacio;
	tablaVacios[posNuevoVacio].posicion = tablaElementos[posDatoAEliminar].posicion;
	tablaVacios[posNuevoVacio].estaVacio = 0;
	tablaVacios[posNuevoVacio].tamanio = espacioVacio;

	tablaElementos[posDatoAEliminar].ID = 0;
	tablaElementos[posDatoAEliminar].posicion = 0;
	tablaElementos[posDatoAEliminar].estaVacio = 1;
	tablaElementos[posDatoAEliminar].tamanio = 0;

	//Log obligatorio.
	log_info(logger, "Eliminada partición con posición de inicio %d.", posDatoAEliminar);

	consolidarCache();

	if(frecuenciaCompactacion <= 1 || particionesLiberadas == frecuenciaCompactacion){
		compactarCache();
	}
}

void imprimirTabla(ItemTablaDinamica tabla[], t_log* logger){
	log_info(logger, "MOSTRANDO ELEMENTOS");
	for(int i = 0; i < tamanioTabla; i++){
		if(!tabla[i].estaVacio){
			log_info(logger, "Elemento ID %d, posicion %d,  espacio %d, fecha %s",
					tabla[i].ID, tabla[i].posicion, tabla[i].tamanio, tabla[i].fechaUltimoUso);
		}
	}
}

//SUSCRIPTORES

void* mapearSuscriptorAModulo(void* suscriptor){
	Suscriptor* suscriptorCasteado = (Suscriptor*)suscriptor;

	return (void*)suscriptorCasteado->modulo;
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
			list_add(tablaElementos[i].suscriptoresRecibidos, &suscriptor);
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
	t_list* moduloSuscriptores = list_map(suscriptoresRecibidos, &mapearSuscriptorAModulo);

	return list_contains_int(moduloSuscriptores, suscriptor.modulo);
}



