#ifndef STRUCTS_H_
#define STRUCTS_H_

#include<stdio.h>
#include<stdlib.h>
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
	CAUGHT
} tipoCola;

typedef enum
{
	SUSCRIBER = 1,
	PUBLISHER
} opCode;


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
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

#endif
