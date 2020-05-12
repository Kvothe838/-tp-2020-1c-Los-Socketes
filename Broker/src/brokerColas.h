#ifndef BROKERCOLAS_H_
#define BROKERCOLAS_H_

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>



typedef struct {
	t_list* suscriptores;
	t_list* mensajes;
	//t_queue* colaMensajes;
} ColaConSuscriptores;


typedef struct {
	int ID;
	int IDCorrelativo;
	t_list* suscriptoresRecibidos;
	t_list* suscriptoresEnviados;

} Mensaje;


ColaConSuscriptores* crearColaConSuscriptores();
ColaConSuscriptores* obtenerCola(t_dictionary* diccionario, char* colaClave);
void agregarSuscriptor(t_dictionary* diccionario, char* colaClave, int* nuevoSuscriptor);
void agregarMensaje(t_dictionary* diccionario, char* colaClave, void* mensaje);
void obtenerSuscriptoresPorCola(t_dictionary* diccionario, char* colaClave);



#endif /* BROKERCOLAS_H_ */
