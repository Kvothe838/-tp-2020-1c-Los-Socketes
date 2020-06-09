/*
 * conexion.h
 *
 *  Created on: 7 jun. 2020
 *      Author: utnso
 */

#ifndef CONEXIONTEAM_H_
#define CONEXIONTEAM_H_

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <shared/utils.h>


pthread_t thread;

void iniciar_servidor(char* ip , char* port);
#endif /* CONEXIONTEAM_H_ */
