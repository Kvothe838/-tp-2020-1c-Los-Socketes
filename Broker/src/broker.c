#include <shared/utils.h>
#include "brokerColas.h"
#include "conexionBroker.h"
#include "cache/basicCache.h"
#include "cache/dynamicCache.h"

int main(void) {
	char* ip, *puerto;
	int tamanioCache, tamanioParticionMinima;
	t_log* logger;
	t_config* config;

	logger = iniciar_logger("loggerBroker.log", "Broker");
	config = leer_config("configBroker.config", logger);

	ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger, "IP %s y PUERTO %s", ip, puerto);

	tamanioCache = config_get_int_value(config, "TAMANO_MEMORIA");
	tamanioParticionMinima = config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
	log_info(logger, "Tamaño cache %d y tamanio mínimo %d", tamanioCache, tamanioParticionMinima);

	//crearDiccionario();




	/*int a, *b;
	a = 7;
	setValue(&a, sizeof(int), 0);
	b = getValue(sizeof(int), 0);
	printf("El valor es : %d", *b);*/

	initializeCache(tamanioCache);
	int tableSize = (tamanioCache / tamanioParticionMinima);
	t_dynamic_table_entry tablaElementos[tableSize];
	t_dynamic_table_entry tablaVacios[tableSize];
	initializeTable(tamanioCache, tableSize, tablaElementos, 0);
	initializeTable(tamanioCache, tableSize, tablaVacios, 1);


	log_info(logger, "[VACIO] %d, espacio %d, posicion %d",
			tablaVacios[0].id, tablaVacios[0].size, tablaVacios[0].position);


	int a = 87;
	double b = 7;
	char* mensaje = "HOLA SOY UN STRING MUUUUUUUUUUY LAAAAAAARGO";

	agregarItem(&a, sizeof(int), tableSize, tamanioParticionMinima, tablaElementos, tablaVacios);
	/*
	 * Ocupa 4 bytes, menos que una partición mínima de memoria (32 bytes).
	 * Entonces ocupa la partición completa, usa 4 bytes y el resto (32-4 = 28 bytes) queda como espacio inservible
	 */
	log_info(logger, "Elemento ID %d, espacio %d, posicion %d",
			tablaElementos[0].id, tablaElementos[0].size, tablaElementos[0].position);

	log_info(logger, "[VACIO] ID %d, espacio %d, posicion %d",
			tablaVacios[0].id, tablaVacios[0].size, tablaVacios[0].position);

	agregarItem(&b, sizeof(double), tableSize, tamanioParticionMinima, tablaElementos, tablaVacios);
	//Idem que arriba, ocupa 8 bytes y entonces solo usa 1 partición
	log_info(logger, "Elemento ID %d, espacio %d, posicion %d",
				tablaElementos[1].id, tablaElementos[1].size, tablaElementos[1].position);

	log_info(logger, "[VACIO] ID %d, espacio %d, posicion %d",
			tablaVacios[0].id, tablaVacios[0].size, tablaVacios[0].position);

	agregarItem(mensaje, strlen(mensaje)+1, tableSize, tamanioParticionMinima, tablaElementos, tablaVacios);
	/*
	 * Acá sucede algo especial: ocupa 120 bytes, por lo que ocupa 4
	 * particiones de memoria (32 * 4 = 128 bytes)
	 *
	 * Además, se le suma uno al largo por el centinela.
	 *
	 * */
	log_info(logger, "Elemento %d, espacio %d, posicion %d",
				tablaElementos[2].id, tablaElementos[2].size, tablaElementos[2].position);

	log_info(logger, "[VACIO] ID %d, espacio %d, posicion %d",
			tablaVacios[0].id, tablaVacios[0].size, tablaVacios[0].position);

	agregarItem(&b, sizeof(double), tableSize, tamanioParticionMinima, tablaElementos, tablaVacios);
	log_info(logger, "Elemento %d, espacio %d, posicion %d",
				tablaElementos[3].id, tablaElementos[3].size, tablaElementos[3].position);

	log_info(logger, "[VACIO] ID %d, espacio %d, posicion %d",
			tablaVacios[0].id, tablaVacios[0].size, tablaVacios[0].position);


	//Acá testeo si los datos guardados por las tablas se pueden obtenre correctamente

	int *recibido1 = obtenerItem(0, tablaElementos);

	log_info(logger, "Item %d y su valor %d", tablaElementos[0].id, *recibido1);

	double *recibido2 = obtenerItem(1, tablaElementos);

	log_info(logger, "Item %d y su valor %f", tablaElementos[1].id, *recibido2);

	char* mensajeLaaargo = obtenerItem(2, tablaElementos);

	log_info(logger, "Item %d y su valor %s", tablaElementos[2].id, mensajeLaaargo);

	double *recibido3 = obtenerItem(3, tablaElementos);

	log_info(logger, "Item %d y su valor %f", tablaElementos[3].id, *recibido3);

	//iniciar_servidor(ip, puerto);

	terminar_programa(logger, config);

	return 0;
}
