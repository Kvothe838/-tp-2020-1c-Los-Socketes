/*
 * conexionBinario.h
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#ifndef CONEXIONBINARIO_H_
#define CONEXIONBINARIO_H_

#include <commons/bitarray.h>
#include <stdint.h>

void guardarDatos(t_bitarray* bitarray);
t_bitarray* obtenerDatos(int tamanio);
uint32_t obtenerBloqueLibre(uint32_t blockCantBytes);
void limpiarBloque(uint32_t posicion, uint32_t blockCantBytes);

#endif /* CONEXIONBINARIO_H_ */
