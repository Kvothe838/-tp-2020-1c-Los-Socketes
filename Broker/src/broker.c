#include <shared/utils.h>
#include "brokerColas.h"
#include "conexionBroker.h"
#include "cache/genericCache.h"
#include <commons/process.h>

static void	sig_usr(int);
static void sig_usr2(int);

void  err_sys(char * msg) {
  printf("%s \n", msg);
  exit(-1);
}

int main(void) {
	t_config* config;
	pthread_t threadIniciarServidor, threadEnviarMensajesSuscriptores;
	IniciarServidorArgs argumentos;

	logger = iniciar_logger("loggerBroker.log", "Broker");
	config = leer_config("configBroker.config", logger);

	argumentos.ip = config_get_string_value(config, "IP_BROKER");
	argumentos.puerto = config_get_string_value(config, "PUERTO_BROKER");

	inicializarDataBasica(config, logger);
	crearDiccionario();

	log_info(logger, "ID del proceso Broker: %d", process_getpid());

	//Forma de llamar al signal: kill -SIGUSR1 [ID]

	if(signal(SIGUSR1, sig_usr) == SIG_ERR)
		err_sys("can't catch SIGUSR1");
	if(signal(SIGUSR2, sig_usr2) == SIG_ERR)
		err_sys("can't catch SIGUSR2");
	/*Esta signal SIGUSR2 sirve sólo para terminar el programa de forma correcta, pasando por las liberaciones de
	 memoria del final del main.*/

	//Pruebas para Buddy System.

	int a = 87;
	double b = 7;
	char* mensaje = "HOLA. TENGO MÁS DE 64 CARACTERES PORQUE SOY RE CAPO Y LA TENGO ENORME.";
	log_info(logger, "Tamaño string: %d", strlen(mensaje));

	agregarItemBuddySystem(mensaje, string_length(mensaje) * 3 + 1, 111, 222, CATCH); //Hardocdeo para simular un dato enorme
	agregarItemBuddySystem(mensaje, string_length(mensaje) * 3 + 1, 111, 222, CATCH); //Hardocdeo para simular un dato enorme
	agregarItemBuddySystem(mensaje, string_length(mensaje) * 3 + 1, 111, 222, CATCH); //Hardocdeo para simular un dato enorme
	agregarItemBuddySystem(mensaje, string_length(mensaje) * 3 + 1, 111, 222, CATCH); //Hardocdeo para simular un dato enorme
	agregarItemBuddySystem(&a, sizeof(int), 0, 0, NEW);
	agregarItemBuddySystem(&b, sizeof(double), 5, 3, GET);
	agregarItemBuddySystem(&a, 7, 0, 0, GET);

	imprimirCache();
	loggerAriel = iniciar_logger("ARIEL.log", "BuddySystem");
	eliminarVictimaBuddySystem();
	eliminarVictimaBuddySystem();
	eliminarVictimaBuddySystem();
	imprimirCache();

	/*eliminarVictimaBuddySystem();

	log_info(logger, "------------------------------------------------------");

	imprimirCache();

	eliminarVictimaBuddySystem();

	log_info(logger, "------------------------------------------------------");

	imprimirCache();*/

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

	eliminarItem(2);

	while(1){
		printf("%d\n", process_getpid());
		sleep(2);
	}

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

	/*pthread_create(&threadIniciarServidor, NULL,(void*)iniciarServidor, (void*)&argumentos);
	pthread_create(&threadEnviarMensajesSuscriptores, NULL,(void*)enviarMensajesSuscriptores, NULL);
	pthread_join(threadIniciarServidor, NULL);
	pthread_join(threadEnviarMensajesSuscriptores, NULL);

	liberarDiccionario();
	liberarCache();
	terminar_programa(logger, config);*/

	return 0;
}

static void sig_usr(int signo)
{
	if(signo == SIGUSR1)
		obtenerDump();
}

static void sig_usr2(int signo){
	log_info(logger, "SIGUSR2");
	if(signo == SIGUSR2)
		cortarPrograma();
}
