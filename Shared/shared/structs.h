#ifndef STRUCTS_H_
#define STRUCTS_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<pthread.h>
#include<commons/collections/list.h>

typedef enum
{
	BROKER = 1,
	GAMECARD,
	GAMEBOY,
	TEAM //No va a haber nadie que use Team, pero desde 4 en adelante van a ser los hilos del Team.
} TipoModulo;

typedef enum
{
	BROKER_GAMEBOY = 1,
	GAMECARD_GAMEBOY,
	TEAM_GAMEBOY,
	SUSCRIPTOR_GAMEBOY
} ModuloGameboy;

typedef enum
{
	NEW = 1,
	GET,
	CATCH,
	APPEARED,
	LOCALIZED,
	CAUGHT
} TipoCola;

typedef enum
{
	SUSCRIBER = 1,
	PUBLISHER,
	ACK,
	ID_MENSAJE,
	NUEVO_MENSAJE_SUSCRIBER
} OpCode;

typedef struct
{
	int size;
	void* stream;
} Buffer;

typedef struct
{
	OpCode codigoOperacion;
	void* contenido;
} Paquete;

typedef struct
{
	TipoModulo modulo;
	void* contenido;
} ContenidoHaciaBroker;

typedef struct
{
	long IDMensaje;
} Ack;

typedef struct
{
	int cantidadColas;
	TipoCola* colas;
} Suscripcion;

typedef struct
{
	long IDCorrelativo;
	TipoCola cola;
	int tamanioDato;
	void* dato;
} Publicacion;

typedef struct
{
	long IDMensaje;
	TipoCola cola;
} IDMensajePublisher;

typedef struct
{
	char* nombre;
	uint32_t largoNombre;
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidad;
} NewPokemon;

typedef struct
{
	char* nombre;
	uint32_t largoNombre;
	uint32_t posX;
	uint32_t posY;
} AppearedPokemon;

typedef struct
{
	char* nombre;
	uint32_t largoNombre;
	uint32_t posX;
	uint32_t posY;
} CatchPokemon;

typedef struct
{
	uint32_t loAtrapo;
} CaughtPokemon;

typedef struct
{
	char* nombre;
	uint32_t largoNombre;
} GetPokemon;

typedef struct {
	long ID;
	long IDMensajeCorrelativo;
	TipoCola cola;
	int tamanioContenido;
	void* contenido;
} MensajeParaSuscriptor; //Mensaje que recibe el suscriptor de una cola.

typedef struct
{
	char* nombre;
	uint32_t largoNombre;
	uint32_t cantidadDeParesDePosiciones;
	t_list* posiciones; //formato: primero X, luego Y y así respectivamente dependiendo la cantidad de pares posiciones
} LocalizedPokemon;

/*Queda como suscriptor pero en realidad se está usando en serveClient de conexionBroker.c como un módulo que se conecta
simplemente, no tiene que ser sí o sí un suscriptor, puede ser un publisher.*/
typedef struct
{
	TipoModulo modulo;
	int socket;
	int estaCaido;
} Suscriptor;

typedef struct {
	char* ip;
	char* puerto;
} IniciarServidorArgs;

void* funcionVacia();

#endif
