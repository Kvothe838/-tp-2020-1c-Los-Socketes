#include "brokerColas.h"

t_dictionary* diccionario;

void crearDiccionario(){
	pthread_mutex_init(&mutexSemaforo, NULL);

	diccionario = dictionary_create();
	dictionary_put(diccionario, tipoColaToString(NEW), crearColaConSuscriptores());
	dictionary_put(diccionario, tipoColaToString(GET), crearColaConSuscriptores());
	dictionary_put(diccionario, tipoColaToString(CATCH), crearColaConSuscriptores());
	dictionary_put(diccionario, tipoColaToString(APPEARED), crearColaConSuscriptores());
	dictionary_put(diccionario, tipoColaToString(LOCALIZED), crearColaConSuscriptores());
	dictionary_put(diccionario, tipoColaToString(CAUGHT), crearColaConSuscriptores());
}

void destructorDataDiccionario(void* colaBuffer)
{
	ColaConSuscriptores* cola = (ColaConSuscriptores*)colaBuffer;

	list_destroy(cola->suscriptores);
	list_destroy_and_destroy_elements(cola->IDMensajes, free);

	free(cola);
}

void liberarDiccionario(){
	dictionary_destroy_and_destroy_elements(diccionario, destructorDataDiccionario);
}

ColaConSuscriptores* crearColaConSuscriptores(){
	pthread_mutex_lock(&mutexSemaforo);

	ColaConSuscriptores* cola = (ColaConSuscriptores*)malloc(sizeof(ColaConSuscriptores));
	cola->suscriptores = list_create();
	cola->IDMensajes = list_create();

	pthread_mutex_unlock(&mutexSemaforo);
	return cola;
}

ColaConSuscriptores* obtenerCola(TipoCola colaClave){
	return dictionary_get(diccionario, tipoColaToString(colaClave));
}

int agregarSuscriptor(TipoCola colaClave, Suscriptor* nuevoSuscriptor){
	int resultado;
	pthread_mutex_lock(&mutexSemaforo);

	ColaConSuscriptores* colaEspecifica = obtenerCola(colaClave);

	_Bool buscarSuscriptor(void* suscriptorGeneric){
		Suscriptor* suscriptor = (Suscriptor*)suscriptorGeneric;

		return suscriptor->modulo == nuevoSuscriptor->modulo;
	}

	void* suscriptorEncontrado = list_find(colaEspecifica->suscriptores, &buscarSuscriptor);

	if(suscriptorEncontrado == NULL)
	{
		list_add(colaEspecifica->suscriptores, nuevoSuscriptor);

		resultado = 1;
	} else {
		Suscriptor* suscriptorAModificar = (Suscriptor*)suscriptorEncontrado;
		suscriptorAModificar->socket = nuevoSuscriptor->socket;
		suscriptorAModificar->estaCaido = 0;

		resultado = 0;
	}

	pthread_mutex_unlock(&mutexSemaforo);

	return resultado;
}

void agregarMensaje(TipoCola colaClave, long* IDMensaje){
	pthread_mutex_lock(&mutexSemaforo);

	ColaConSuscriptores* colaEspecifica = obtenerCola(colaClave);
	list_add(colaEspecifica->IDMensajes, IDMensaje);

	pthread_mutex_unlock(&mutexSemaforo);
}

void imprimir(int a){
	printf("Suscriptor %d \n", a);
}

t_list* obtenerSuscriptoresPorCola(TipoCola colaClave){
	pthread_mutex_lock(&mutexSemaforo);

	ColaConSuscriptores* colaEspecifica = obtenerCola(colaClave);

	pthread_mutex_unlock(&mutexSemaforo);

	return colaEspecifica->suscriptores;
}
/*
void eliminarMensajeDeLista(int IDMensaje, TipoCola colaClave)
{
	//sem_wait(semColaMensajes);
	//log_info(loggerObligatorio, "Elimino mensaje de lista. ID: %ld, cola: %s", IDMensaje, tipoColaToString(colaClave));
	ColaConSuscriptores* colaEspecifica = obtenerCola(colaClave);
	//log_info(loggerObligatorio, "Cola obtenida");
	int indiceEliminar = -1;

	for(int i = 0; i < list_size(colaEspecifica->IDMensajes); i++)
	{
		//log_info(loggerObligatorio, "Itero cola");
		long* ID = list_get(colaEspecifica->IDMensajes, i);
		//log_info(loggerObligatorio, "Obtengo Id: %ld", *ID);

		if(IDMensaje == *ID)
		{
			//log_info(loggerObligatorio, "Indice encontrado: %d", i);
			indiceEliminar = i;
		}
	}

	if(indiceEliminar != -1)
	{
		log_info(loggerObligatorio, "Eliminado mensaje %ld", IDMensaje);
		list_remove(colaEspecifica->IDMensajes, indiceEliminar);
	}

	//log_info(loggerObligatorio, "Salgo de eliminar mensaje de lista");

	//sem_post(semColaMensajes);
}*/

