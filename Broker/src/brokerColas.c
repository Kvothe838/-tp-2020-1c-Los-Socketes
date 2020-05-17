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
	cola->mensajes = list_create();
	return cola;
}

ColaConSuscriptores* obtenerCola(TipoCola colaClave){
	return dictionary_get(diccionario, tipoColaToString(colaClave));
}

void agregarSuscriptor(TipoCola colaClave, int nuevoSuscriptor){
	ColaConSuscriptores* colaEspecifica = obtenerCola(colaClave);
	list_add(colaEspecifica->suscriptores, (void*)nuevoSuscriptor);
}

void agregarMensaje(TipoCola colaClave, void* mensaje){
	ColaConSuscriptores* colaEspecifica = obtenerCola(colaClave);
	list_add(colaEspecifica->mensajes, mensaje);
}

void imprimir(int a){
	printf("Suscriptor %d \n", a);
}

void obtenerSuscriptoresPorCola(TipoCola colaClave){
	ColaConSuscriptores* colaEspecifica = obtenerCola(colaClave);
	list_iterate(colaEspecifica->suscriptores, (void*)imprimir);
}

