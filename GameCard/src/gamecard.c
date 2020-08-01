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

	inicializarData(logger);

	//log_info(logger, "ID del proceso GameCard: %d", process_getpid());

	ipBroker = config_get_string_value(config, "IP_BROKER");
	puertoBroker = config_get_string_value(config, "PUERTO_BROKER");

	/*LocalizedPokemon* pikachu = getLocalized("Pikachu", 4, 1, 1, 1, 1);
	funcionDePruebaParaGabo(logger, ipBroker, puertoBroker, pikachu);
	LocalizedPokemon* squirtle = getLocalized("Squirtle", 1, 3, 1);
	funcionDePruebaParaGabo(logger, ipBroker, puertoBroker, squirtle);*/



	int conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);

	int suscripcionEnviada = enviarSuscripcion(conexionBroker, GAMECARD, 3, NEW, GET, CATCH);

	if(suscripcionEnviada)
		iniciar_servidor(config, conexionBroker);

	/*inicializarData(logger);
	for (int i = 1; i < 5; ++i)
		administrarNewPokemon("TesteoLoco", i*100000000, i*100000000, 3);
	administrarNewPokemon("TesteoLoco", 1, 1, 1);
	administrarCatchPokemon("TesteoLoco", 400000000, 400000000);
	for (int i = 1; i < 100; ++i)
		administrarNewPokemon("Pikachu", i, i, 3);
	LocalizedPokemon * datosRecibidos = administrarGetPokemon("Pikachu");
	printf("%s\n", datosRecibidos->nombre);
	printf("%d\n", datosRecibidos->cantidadDePosiciones);
	uint32_t offset = 0;
	uint32_t *data;
	uint32_t ciclos = datosRecibidos->cantidadDePosiciones;
	while(ciclos != 0){
		ciclos--;
		memcpy(&data, (datosRecibidos->data + offset), sizeof(uint32_t));
		printf("X:%d - ", (int)data);
		offset += sizeof(uint32_t);
		memcpy(&data, (datosRecibidos->data + offset), sizeof(uint32_t));
		printf("Y:%d - ", (int)data);
		offset += sizeof(uint32_t);
		memcpy(&data, (datosRecibidos->data + offset), sizeof(uint32_t));
		printf("Cantidad:%d\n", (int)data);
		offset += sizeof(uint32_t);
	}
	free(datosRecibidos->data);
	free(datosRecibidos);*/

	//Para pruebas de suscripción con Broker. Comentar si no se usa pero no borrar plis.


	/*ip = config_get_string_value(config, "IP_BROKER");
	puerto = config_get_string_value(config, "PUERTO_BROKER");
	funcionDePruebaParaGabo(logger, ip, puerto);*/
	//int socketCliente = crear_conexion_cliente(ip, puerto);
	/*enviarSuscripcion(socketCliente, GAMECARD, 3, NEW, GET, CAUGHT);
	while(1){
		OpCode codigo;
		recv(socketCliente, &codigo, sizeof(OpCode), 0);
		if(codigo == NUEVO_MENSAJE_SUSCRIBER){
			MensajeParaSuscriptor* mensaje;
			int recepcionExitosa = recibirMensajeSuscriber(socketCliente, logger, GAMECARD, &mensaje, ip, puerto);
			if(recepcionExitosa)
			{
				free(mensaje);
			}
		}
	}*/

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
