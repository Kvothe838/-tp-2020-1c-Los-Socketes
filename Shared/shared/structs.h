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
//#include<inttypes.h>

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
	PUBLISHER
} OpCode;


/*


 * SUSCRIPCION (se suscribe a una cola)
	opCode queCarajoHace; //SUSCRIBER o PUBLISHER
	uint32_t size;
	typedef struct
	{
		int cantidadQueMeSuscribo;
		tipoCola* cantidad;
	} inscripcion;



 * PUBLISHER (manda un mensaje a una cola)
	opCode queCarajoHace;
	uint32_t size;
	typedef struct{
		tipoCola aQueCOla;
		... //resto de datos
		... //resto de datos
		... //resto de datos
	} especifica;


*/

typedef struct
{
	int cantidadColas;
	TipoCola* colas;
} Suscripcion;

typedef struct
{
	int size;
	void* stream;
} Buffer;

typedef struct
{
	OpCode codigoOperacion;
	Buffer* buffer;
} Paquete;

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

#endif
