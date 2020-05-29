#include <shared/utils.h>
#include "brokerColas.h"
#include "conexionBroker.h"
#include "cache/basicCache.h"
#include "cache/dynamicCache.h"

void imprimirTabla(int tamanioTabla, t_dynamic_table_entry tabla[], t_log* logger){
	log_info(logger, "MOSTRANDO OCUPADOS");
	for(int i = 0; i < tamanioTabla; i++){
		if(!tabla[i].isEmpty){
			log_info(logger, "Elemento ID %d, posicion %d,  espacio %d, fecha %s",
					tabla[i].id, tabla[i].position, tabla[i].size, tabla[i].dateBorn);
		}
	}
}

int main(void) {
	char* ip, *puerto, *algoritmoEleccion;
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
	algoritmoEleccion = config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE");
	log_info(logger, "Tamaño cache %d y tamanio mínimo %d", tamanioCache, tamanioParticionMinima);

	//crearDiccionario();


	initializeCache(tamanioCache);
	int tableSize = (tamanioCache / tamanioParticionMinima);
	t_dynamic_table_entry tablaElementos[tableSize], tablaVacios[tableSize], tablaElementosCompacta[tableSize];
	initializeTable(tamanioCache, tableSize, tablaElementos, 0);
	initializeTable(tamanioCache, tableSize, tablaVacios, 1);


	log_info(logger, "[VACIO] %d, espacio %d, posicion %d",
			tablaVacios[0].id, tablaVacios[0].size, tablaVacios[0].position);


	int a = 87;
	double b = 7;
	char* mensaje = "HOLA SOY UN STRING MUUUUUUUUUUY LAAAAAAARGO";

	agregarItem(&a, sizeof(int), tableSize, tamanioParticionMinima, algoritmoEleccion,
			tablaElementos, tablaVacios);
	//log_info(logger, "Primer elemento fecha %s", tablaElementos[0].dateBorn);

	/*
	 * Ocupa 4 bytes, menos que una partición mínima de memoria (32 bytes).
	 * Entonces ocupa la partición completa, usa 4 bytes y el resto (32-4 = 28 bytes) queda como espacio inservible
	 */

	agregarItem(&b, sizeof(double), tableSize, tamanioParticionMinima, algoritmoEleccion,
			tablaElementos, tablaVacios);
	//Idem que arriba, ocupa 8 bytes y entonces solo usa 1 partición

	agregarItem(mensaje, strlen(mensaje)+1, tableSize, tamanioParticionMinima, algoritmoEleccion,
			tablaElementos, tablaVacios);
	/*
	 * Acá sucede algo especial: ocupa 120 bytes, por lo que ocupa 4
	 * particiones de memoria (32 * 4 = 128 bytes)
	 *
	 * Además, se le suma uno al largo por el centinela.
	 *
	 * */

	agregarItem(&b, sizeof(double), tableSize, tamanioParticionMinima, algoritmoEleccion,
			tablaElementos, tablaVacios);



	imprimirTabla(tableSize, tablaElementos, logger);
	imprimirTabla(tableSize, tablaVacios, logger);


	//Acá testeo si los datos guardados por las tablas se pueden obtenre correctamente

	int *recibido1 = obtenerItem(0, tablaElementos);

	log_info(logger, "Item %d y su valor %d", tablaElementos[0].id, *recibido1);

	double *recibido2 = obtenerItem(1, tablaElementos);

	log_info(logger, "Item %d y su valor %f", tablaElementos[1].id, *recibido2);

	char* mensajeLaaargo = obtenerItem(2, tablaElementos);

	log_info(logger, "Item %d y su valor %s", tablaElementos[2].id, mensajeLaaargo);

	double *recibido3 = obtenerItem(3, tablaElementos);

	log_info(logger, "Item %d y su valor %f", tablaElementos[3].id, *recibido3);

	eliminarItem(0, tableSize, tamanioParticionMinima, tablaElementos, tablaVacios);

	agregarItem(&b, sizeof(double), tableSize, tamanioParticionMinima, algoritmoEleccion,
				tablaElementos, tablaVacios);


	imprimirTabla(tableSize, tablaElementos, logger);
	imprimirTabla(tableSize, tablaVacios, logger);


	terminar_programa(logger, config);

	return 0;
}



