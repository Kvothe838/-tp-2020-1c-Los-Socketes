#ifndef CONEXIONBINARIO_H_
#define CONEXIONBINARIO_H_

#include <commons/bitarray.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void guardarDatos(t_bitarray* bitarray, char* puntoMontaje);
t_bitarray* obtenerDatos(int tamanio, char* puntoMontaje);
uint32_t obtenerBloqueLibre(uint32_t blockCantBytes, char* puntoMontaje);
void limpiarBloque(uint32_t posicion, uint32_t blockCantBytes, char* puntoMontaje);

#endif /* CONEXIONBINARIO_H_ */
