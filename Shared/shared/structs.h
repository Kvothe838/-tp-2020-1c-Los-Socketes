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

typedef enum
{
	NEW = 1,
	GET,
	CATCH,
	APPEARED,
	LOCALIZED,
	CAUGHT,
	SUSCRIBER,
	PUBLISHER
} op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

//ESTRUCTURAS
//DEL BROKER
typedef struct
{
	char* nombre;
	uint_32 largoNombre;
	uint_32 posX;
	uint_32 posY;
	uint_32 cantidad;
} t_NewPokemon;

typedef struct
{
	char* nombre;
	uint_32 largoNombre;
	uint_32 posX;
	uint_32 posY;
} t_AppearedPokemon;

typedef struct
{
	char* nombre;
	uint_32 largoNombre;
	uint_32 posX;
	uint_32 posY;
} t_CatchPokemon;

typedef struct
{
	uint_32 loAtrapo;
} t_CaughtPokemon;

typedef struct
{
	char* nombre;
	uint_32 largoNombre;
} t_GetPokemon;

//ESTRUCTURAS
//DEL TEAM
typedef struct
{
	char* nombre;
	uint_32 largoNombre;
	uint_32 posX;
	uint_32 posY;
} t_AppearedPokemon;

#endif
