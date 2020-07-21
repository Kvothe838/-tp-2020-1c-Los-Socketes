#include <shared/utils.h>
#include "brokerColas.h"
#include "conexionBroker.h"
#include "cache/dynamicCache.h"

int main(void) {
	t_config* config;
	//pthread_t threadIniciarServidor, threadEnviarMensajesSuscriptores;
	IniciarServidorArgs argumentos;

	logger = iniciar_logger("loggerBroker.log", "Broker");
	config = leer_config("configBroker.config", logger);

	argumentos.ip = config_get_string_value(config, "IP_BROKER");
	argumentos.puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "Escuchando en ip %s y puerto %s...", argumentos.ip, argumentos.puerto);

	inicializarDataBasica(config, logger);
	crearDiccionario();


	//Pruebas para la caché.
	/*int a = 87;
	double b = 7;
	char* mensaje = "HOLA SOY UN STRING MUUUUUUUUUUY LAAAAAAARGO";
	int hasta = 5;

	agregarItem(&a, sizeof(int), 0, NULL, NEW);
	agregarItem(&b, sizeof(double), 1, NULL, NEW);
	agregarItem(mensaje, strlen(mensaje)+1, 2, NULL, NEW);
	agregarItem(&b, sizeof(double), 3, NULL, NEW);

	log_info(logger, "ANTES");

	for (int i = 0; i <= hasta; i++){
		log_info(logger, "[ELEMENTO] ID %d, tamaño %d, posicion %d",
		tablaElementos[i].ID, tablaElementos[i].tamanio, tablaElementos[i].posicion);
	}

	for (int i = 0; i <= hasta; i++){
		log_info(logger, "[VACIO] ID %d, tamaño %d, posicion %d",
		tablaVacios[i].ID, tablaVacios[i].tamanio, tablaVacios[i].posicion);
	}


	eliminarItem(1);

	log_info(logger, "ELIMINADO ITEM 1");

	for (int i = 0; i <= hasta; i++){
		log_info(logger, "[ELEMENTO] ID %d, tamaño %d, posicion %d",
		tablaElementos[i].ID, tablaElementos[i].tamanio, tablaElementos[i].posicion);
	}

	for (int i = 0; i <= hasta; i++){
		log_info(logger, "[VACIO] ID %d, tamaño %d, posicion %d",
		tablaVacios[i].ID, tablaVacios[i].tamanio, tablaVacios[i].posicion);
	}

	eliminarItem(3);

	log_info(logger, "ELIMINADO ITEM 3");

	for (int i = 0; i <= hasta; i++){
		log_info(logger, "[ELEMENTO] ID %d, tamaño %d, posicion %d",
		tablaElementos[i].ID, tablaElementos[i].tamanio, tablaElementos[i].posicion);
	}

	for (int i = 0; i <= hasta; i++){
		log_info(logger, "[VACIO] ID %d, tamaño %d, posicion %d",
		tablaVacios[i].ID, tablaVacios[i].tamanio, tablaVacios[i].posicion);
	}

	agregarItem(&a, sizeof(int), 4, NULL, NEW);

	log_info(logger, "AGREGADO ITEM 4");

	for (int i = 0; i <= hasta; i++){
		log_info(logger, "[ELEMENTO] ID %d, tamaño %d, posicion %d",
		tablaElementos[i].ID, tablaElementos[i].tamanio, tablaElementos[i].posicion);
	}

	for (int i = 0; i <= hasta; i++){
		log_info(logger, "[VACIO] ID %d, tamaño %d, posicion %d",
		tablaVacios[i].ID, tablaVacios[i].tamanio, tablaVacios[i].posicion);
	}


	compactarCache();

	log_info(logger, "COMPACTADO");

	for (int i = 0; i <= hasta; i++){
		log_info(logger, "[ELEMENTO] ID %d, tamaño %d, posicion %d",
		tablaElementos[i].ID, tablaElementos[i].tamanio, tablaElementos[i].posicion);
	}

	for (int i = 0; i <= hasta; i++){
		log_info(logger, "[VACIO] ID %d, tamaño %d, posicion %d",
		tablaVacios[i].ID, tablaVacios[i].tamanio, tablaVacios[i].posicion);
	}*/

	inicializarMutex();
	manejarPublisher(-1);

	//pthread_create(&threadIniciarServidor, NULL,(void*)iniciarServidor, (void*)&argumentos);
	//pthread_create(&threadEnviarMensajesSuscriptores, NULL,(void*)enviarMensajesSuscriptores, NULL);
	//pthread_join(threadIniciarServidor, NULL);
	//pthread_join(threadEnviarMensajesSuscriptores, NULL);

	terminar_programa(logger, config);

	return 0;
}




