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

	ipBroker = config_get_string_value(config, "IP_BROKER");
	puertoBroker = config_get_string_value(config, "PUERTO_BROKER");
	argumentosGameboy.ip = config_get_string_value(config, "IP_GAMECARD");
	argumentosGameboy.puerto = config_get_string_value(config, "PUERTO_GAMECARD");

	/*LocalizedPokemon* pikachu = getLocalized("Pikachu", 4, 1, 1, 1, 1);
	funcionDePruebaParaGabo(logger, ipBroker, puertoBroker, pikachu);
	LocalizedPokemon* squirtle = getLocalized("Squirtle", 1, 3, 1);
	funcionDePruebaParaGabo(logger, ipBroker, puertoBroker, squirtle);*/
	pthread_create(&threadEscucharGameboy, NULL,(void*)iniciarServidorGameboy, (void*)&argumentosGameboy);
	pthread_detach(threadEscucharGameboy);

	int conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);

	int suscripcionEnviada = enviarSuscripcion(conexionBroker, GAMECARD, 3, NEW, GET, CATCH);

	if(suscripcionEnviada)
		iniciar_servidor(config, conexionBroker);

	//pthread_join(threadEscucharGameboy, NULL);

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
