#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include "conexionBinario/conexionBinario.h"
//#include "ManejoDeBloques/manejoDeArchivos.h"
#include <stdlib.h>
#include "conexionGameCard.h"
#include "ManejoDeBloques/manejoDeArchivos.h"
#include <commons/process.h>

void funcionDePruebaParaGabo(t_log* logger, char* ip, char* puerto, LocalizedPokemon* pokemon);



int main(void) {
	t_log* logger = iniciar_logger("gamecard.log", "GAMECARD");
	t_config* config = leer_config("GameCard.config", logger);
	pthread_t threadEscucharGameboy;
	IniciarServidorArgs argumentosGameboy;

	inicializarData(logger);

	//log_info(logger, "ID del proceso GameCard: %d", process_getpid());

	log_info(logger, "1");
	ipBroker = config_get_string_value(config, "IP_BROKER");
	log_info(logger, "2");
	puertoBroker = config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger, "3");
	argumentosGameboy.ip = config_get_string_value(config, "IP_GAMECARD");
	log_info(logger, "4");
	argumentosGameboy.puerto = config_get_string_value(config, "PUERTO_GAMECARD");
	log_info(logger, "5");

	/*LocalizedPokemon* pikachu = getLocalized("Pikachu", 4, 1, 1, 1, 1);
	funcionDePruebaParaGabo(logger, ipBroker, puertoBroker, pikachu);
	LocalizedPokemon* squirtle = getLocalized("Squirtle", 1, 3, 1);
	funcionDePruebaParaGabo(logger, ipBroker, puertoBroker, squirtle);*/
	log_info(logger, "6");
	pthread_create(&threadEscucharGameboy, NULL,(void*)iniciarServidorGameboy, (void*)&argumentosGameboy);
	log_info(logger, "7");
	//crearDiccionarioSemaforo();

	log_info(logger, "8");
	int conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);
	log_info(logger, "9");

	log_info(logger, "10");
	int suscripcionEnviada = enviarSuscripcion(conexionBroker, GAMECARD, 3, NEW, GET, CATCH);
	log_info(logger, "11");

	log_info(logger, "12");
	fflush(stdout);
	if(suscripcionEnviada)
		iniciar_servidor(config, conexionBroker);

	log_info(logger, "13");
	pthread_join(threadEscucharGameboy, NULL);

	//liberarVariablesGlobales();

	log_destroy(logger);
	//terminar_programa(logger, config);

	return EXIT_SUCCESS;
}

void funcionDePruebaParaGabo(t_log* logger, char* ip, char* puerto, LocalizedPokemon* pokemon){
	int conexionBroker = crear_conexion_cliente(ip, puerto);

	IDMensajePublisher* respuesta;
	if(!enviarPublisherSinIDCorrelativo(logger, conexionBroker, GAMECARD, pokemon, LOCALIZED, &respuesta))
	{
		log_info(logger, "ERROR - No se pudo enviar el mensaje.");
		abort();
	}

	log_info(logger, "OK - Se envió correctamente el mensaje.");

	free(pokemon);

	liberar_conexion_cliente(conexionBroker);
}
