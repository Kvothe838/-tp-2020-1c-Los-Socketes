#include "brokerColas.h"

t_dictionary* diccionario;

void crearDiccionario(){
	diccionario = dictionary_create();
	dictionary_put(diccionario, tipoColaToString(NEW), crearColaConSuscriptores());
	dictionary_put(diccionario, tipoColaToString(GET), crearColaConSuscriptores());
	dictionary_put(diccionario, tipoColaToString(CATCH), crearColaConSuscriptores());
	dictionary_put(diccionario, tipoColaToString(APPEARED), crearColaConSuscriptores());
	dictionary_put(diccionario, tipoColaToString(LOCALIZED), crearColaConSuscriptores());
	dictionary_put(diccionario, tipoColaToString(CAUGHT), crearColaConSuscriptores());
}

ColaConSuscriptores* crearColaConSuscriptores(){
	ColaConSuscriptores* cola = (ColaConSuscriptores*)malloc(sizeof(ColaConSuscriptores));
	cola->suscriptores = list_create();
	cola->IDMensajes = list_create();
	return cola;
}

ColaConSuscriptores* obtenerCola(TipoCola colaClave){
	return dictionary_get(diccionario, tipoColaToString(colaClave));
}

int agregarSuscriptor(TipoCola colaClave, Suscriptor* nuevoSuscriptor){
	ColaConSuscriptores* colaEspecifica = obtenerCola(colaClave);

	_Bool buscarSuscriptor(void* suscriptorGeneric){
		Suscriptor* suscriptor = (Suscriptor*)suscriptorGeneric;

		return suscriptor->modulo == nuevoSuscriptor->modulo;
	}

	void* suscriptorEncontrado = list_find(colaEspecifica->suscriptores, &buscarSuscriptor);

	if(suscriptorEncontrado == NULL)
	{
		list_add(colaEspecifica->suscriptores, nuevoSuscriptor);

		return 1;
	} else {
		Suscriptor* suscriptorAModificar = (Suscriptor*)suscriptorEncontrado;

		suscriptorAModificar->socket = nuevoSuscriptor->socket;

		return 0;
	}
}

void agregarMensaje(TipoCola colaClave, long* IDMensaje){
	ColaConSuscriptores* colaEspecifica = obtenerCola(colaClave);
	list_add(colaEspecifica->IDMensajes, IDMensaje);
}

void imprimir(int a){
	printf("Suscriptor %d \n", a);
}

t_list* obtenerSuscriptoresPorCola(TipoCola colaClave){
	ColaConSuscriptores* colaEspecifica = obtenerCola(colaClave);
	return colaEspecifica->suscriptores;
}

