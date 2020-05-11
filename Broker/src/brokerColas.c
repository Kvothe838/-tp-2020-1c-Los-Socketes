#include "brokerColas.h"

t_dictionary* crearDiccionario(){
	t_dictionary* diccionarioPrincipal = dictionary_create();
	dictionary_put(diccionarioPrincipal, "NEW", crearColaConSuscriptores());
	dictionary_put(diccionarioPrincipal, "GET", crearColaConSuscriptores());
	dictionary_put(diccionarioPrincipal, "CATCH", crearColaConSuscriptores());
	dictionary_put(diccionarioPrincipal, "APPEARED", crearColaConSuscriptores());
	dictionary_put(diccionarioPrincipal, "LOCALIZED", crearColaConSuscriptores());
	dictionary_put(diccionarioPrincipal, "CAUGHT", crearColaConSuscriptores());
	return diccionarioPrincipal;
}

ColaConSuscriptores* crearColaConSuscriptores(){
	ColaConSuscriptores* cola = malloc(sizeof(ColaConSuscriptores));
	cola->suscriptores = list_create();
	cola->mensajes = list_create();
	//cola->colaMensajes = queue_create();
	return cola;
}

ColaConSuscriptores* obtenerCola(t_dictionary* diccionario, char* colaClave){
	return dictionary_get(diccionario, colaClave);
}

void agregarSuscriptor(t_dictionary* diccionario, char* colaClave, int* nuevoSuscriptor){
	ColaConSuscriptores* colaEspecifica = obtenerCola(diccionario, colaClave);
	list_add((colaEspecifica->suscriptores), nuevoSuscriptor);
}

void agregarMensaje(t_dictionary* diccionario, char* colaClave, void* mensaje){
	ColaConSuscriptores* colaEspecifica = obtenerCola(diccionario, colaClave);
	queue_push(colaEspecifica->mensajes, mensaje);
}

void imprimir(int a){
	printf("Suscriptores %d \n", a);
}

void obtenerSuscriptoresPorCola(t_dictionary* diccionario, char* colaClave){
	ColaConSuscriptores* colaEspecifica = obtenerCola(diccionario, colaClave);
	list_iterate(colaEspecifica->suscriptores, (void*)imprimir);
}

