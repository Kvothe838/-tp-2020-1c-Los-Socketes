#ifndef MESSAGES_H_
#define MESSAGES_H_

#include "../shared/structs.h"
#include "../shared/connection.h"

int enviarSuscripcion(int socket, TipoModulo modulo, int cantidadColasASuscribir, ...);
int enviarMensajeASuscriptor(int socketSuscriptor, long ID, TipoCola cola, void* data);
void* recibirMensaje(int socket_cliente);
int enviarPublisherSinIDCorrelativo(t_log* logger, int socket, TipoModulo modulo, void* dato, TipoCola cola,
		IDMensajePublisher** mensajeRecibido);
int enviarPublisherConIDCorrelativo(t_log* logger, int socket, TipoModulo modulo, void* dato, TipoCola cola,
		long IDCorrelativo, IDMensajePublisher** mensajeRecibido);
int recibirMensajeSuscriber(int socket, t_log* logger, TipoModulo modulo, MensajeParaSuscriptor** mensaje,
		char* ip, char* puerto);
int recibirIDMensajePublisher(int socket, IDMensajePublisher* mensaje);

/**
 * Dado un Buffer con size y stream enviado desde socket_cliente, devuelve stream y asigna a size.
 *
 * @socketCliente El socket del cliente que envió el paquete
 * @size Un puntero a una variable vacía para almacenar el tamaño del stream
 * @return El stream dentro del paquete en forma de void*
 */
void* recibirMensajeServidor(int socketCliente, int* size);
char* tipoColaToString(TipoCola tipoCola);
char* tipoModuloToString(TipoModulo modulo);
ModuloGameboy argvToModuloGameboy(char* modulo);
TipoCola argvToTipoCola(char* cola);
NewPokemon* getNewPokemon(char* nombre, int posX, int posY, int cantidad);
AppearedPokemon* getAppearedPokemon(char* nombre, int posX, int posY);
CatchPokemon* getCatchPokemon(char* nombre, int posX, int posY);
CaughtPokemon* getCaughtPokemon(int loAtrapo);
GetPokemon* getGetPokemon(char* nombre);
LocalizedPokemon* getLocalized(char* nombre, int cantidadParesPosiciones, ...);
LocalizedPokemon* getLocalizedConList(char* nombre, int cantidadParesPosiciones, t_list* posiciones);

int recibirAck(int socket, Ack** respuesta);

#endif
