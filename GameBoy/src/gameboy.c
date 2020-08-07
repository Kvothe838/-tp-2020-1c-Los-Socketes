// proyecto Gameboy
// á: 160 é: 130 í: 161 ó: 162 ú: 163 Á: 181 É: 144 Í: 214 Ó: 224 Ú: 23 ñ: 164 Ñ: 165

#include <shared/utils.h>
#include <shared/messages.h>
#include <shared/structs.h>
#include <string.h>

typedef struct {
	char* ip;
	char* puerto;
	int socket;
	t_log* logger;
} ArgumentosEscucharBroker;

void escucharBroker(ArgumentosEscucharBroker* argumentos);

int main(int argc, char **argv){
	int const cantidadMinArgc = 2, cantidadMaxArgc = 20;

	int conexionBroker, conexionGamecard, cantidadTeams;
	char *ipBroker, *ipGamecard, *puertoBroker, *puertoGamecard, **ipsPuertosTeam;

	t_log* logger;
	t_config* config;

	logger = iniciar_logger("loggerGameBoy.log", "GameBoy");
	config = leer_config("configGameBoy.config", logger);

    if(argc == 1){
        log_info(logger, "No se ha ingresado ningún argumento de línea de comando adicional que no sea el nombre del programa");
		log_info(logger, "Formato de ejecución: ./gameboy [BROKER|GAMECARD|TEAM|SUSCRIPTOR] [NEW_POKEMON|APPEARED_POKEMON|CATCH_POKEMON|CAUGHT_POKEMON|GET_POKEMON] [ARGUMENTOS]*");
		exit(-1);
	}

    if(argc < cantidadMinArgc || argc > cantidadMaxArgc)
    {
		log_info(logger, "Por favor, revise que está ingresando como MINIMO %d argumentos, y como MAXIMO %d argumentos, y vuelva a intentar", cantidadMinArgc, cantidadMaxArgc);
		exit(-1);
	}

	IDMensajePublisher* mensajeRecibido;

	switch(argvToModuloGameboy(argv[1]))
	{
		case BROKER_GAMEBOY:;
			ipBroker = config_get_string_value(config, "IP_BROKER");
			puertoBroker = config_get_string_value(config, "PUERTO_BROKER");
			conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);
			if(conexionBroker == 0){
				log_info(logger, "ERROR - No se pudo crear la conexión con %s", argv[1]);
				exit(-1);
			}

			log_info(logger, "Conexión al proceso %s", argv[1]);

			switch(argvToTipoCola(argv[2]))
			{
				case NEW:;
					NewPokemon* newPokemon = getNewPokemon(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6])); //usa malloc, entonces hay que hacer un free

					if(!enviarPublisherSinIDCorrelativo(logger, conexionBroker, GAMEBOY, newPokemon, NEW, &mensajeRecibido))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %d %d %d", argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
						exit(-1);
					}

					liberar_conexion_cliente(conexionBroker);

					free(newPokemon);

					break;

				case APPEARED:;
					AppearedPokemon* appearedPokemon = getAppearedPokemon(argv[3], atoi(argv[4]), atoi(argv[5]));//usa malloc, entonces hay que hacer un free

					if(!enviarPublisherConIDCorrelativo(logger, conexionBroker, GAMEBOY, appearedPokemon, APPEARED, (int)argv[6], &mensajeRecibido))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %d %d", argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
						exit(-1);
					}

					liberar_conexion_cliente(conexionBroker);

					free(appearedPokemon);

					break;

				case CATCH:;
					CatchPokemon* catchPokemon = getCatchPokemon(argv[3], atoi(argv[4]), atoi(argv[5]));//usa malloc, entonces hay que hacer un free

					if(!enviarPublisherSinIDCorrelativo(logger, conexionBroker, GAMEBOY, catchPokemon, CATCH, &mensajeRecibido))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %d %d", argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
						exit(-1);
					}

					liberar_conexion_cliente(conexionBroker);

					free(catchPokemon);

					break;

				case CAUGHT:;
					CaughtPokemon* caughtPokemon = getCaughtPokemon(atoi(argv[3]));//usa malloc, entonces hay que hacer un free

					if(!enviarPublisherSinIDCorrelativo(logger, conexionBroker, GAMEBOY, caughtPokemon, CAUGHT, &mensajeRecibido))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %d", argv[1], argv[2], atoi(argv[3]));
						exit(-1);
					}
					liberar_conexion_cliente(conexionBroker);

					free(caughtPokemon);

					break;

				case GET:;
					GetPokemon* getPokemon = getGetPokemon(argv[3]);//usa malloc, entonces hay que hacer un free

					if(!enviarPublisherSinIDCorrelativo(logger, conexionBroker, GAMEBOY, getPokemon, GET, &mensajeRecibido))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s", argv[1], argv[2], argv[3]);
						exit(-1);
					}

					liberar_conexion_cliente(conexionBroker);

					free(getPokemon);

					break;

				default:
				{
					log_info(logger, "ERROR. No se reconoce el tipo de cola ingresada.");
					exit(-1);
				}
			}

			break;

		case TEAM_GAMEBOY:;
			ipsPuertosTeam = config_get_array_value(config, "IPS_PUERTOS_TEAM");
			cantidadTeams = config_get_int_value(config, "CANTIDAD_TEAMS");

			for(int i = 0; i < cantidadTeams * 2; i = i+2){
				char* ipTeam = ipsPuertosTeam[i];
				char* puertoTeam = ipsPuertosTeam[i+1];

				log_info(logger, "IPTEAM: %s | PUERTOTEAM: %s", ipTeam, puertoTeam);

				int conexionTeam = crear_conexion_cliente(ipTeam, puertoTeam);

				if(conexionTeam == 0){
					log_info(logger, "ERROR - No se pudo crear la conexión con %s", argv[1]);
				}

				log_info(logger, "Conexión al proceso %s", argv[1]);

				switch(argvToTipoCola(argv[2]))
				{
					case APPEARED:;
						AppearedPokemon* pokemon = getAppearedPokemon(argv[3], atoi(argv[4]), atoi(argv[5]));//usa malloc, entonces hay que hacer un free

						if(!enviarMensajeASuscriptor(conexionTeam, 1, APPEARED, pokemon))
						{
							log_info(logger, "ERROR. No se pudo enviar el mensaje %s %s %s %d %d", argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
						}

						liberar_conexion_cliente(conexionTeam);

						free(pokemon);

						break;

					default:
						log_info(logger, "ERROR. No se reconoce el tipo de cola ingresada.");
						exit(-1);
				}
			}

			break;

		case GAMECARD_GAMEBOY:
		{
			ipGamecard = config_get_string_value(config, "IP_GAMECARD");
			puertoGamecard = config_get_string_value(config, "PUERTO_GAMECARD");
			log_info(logger, "IP_GAMECARD %s y PUERTO_GAMECARD %s\n", ipGamecard, puertoGamecard);
			conexionGamecard = crear_conexion_cliente(ipGamecard, puertoGamecard);

			if(conexionGamecard == 0){
				log_info(logger, "ERROR - No se pudo crear la conexión con %s", argv[1]);
				exit(-1);
			}

			log_info(logger, "Conexión al proceso %s", argv[1]);

			switch(argvToTipoCola(argv[2]))
			{
				case CATCH:;
					CatchPokemon* catchPokemon = getCatchPokemon(argv[3], atoi(argv[4]), atoi(argv[5]));//usa malloc, entonces hay que hacer un free
					long IDMensajeCatch = (long)atoi(argv[6]);

					if(!enviarMensajeASuscriptor(conexionGamecard, IDMensajeCatch, CATCH, catchPokemon))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %d %d %ld", argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), IDMensajeCatch);
						exit(-1);
					}

					liberar_conexion_cliente(conexionGamecard);

					free(catchPokemon);

					break;

				case GET:;
					GetPokemon* getPokemon = getGetPokemon(argv[3]);//usa malloc, entonces hay que hacer un free
					long IDMensajeGet = (long)atoi(argv[4]);

					if(!enviarMensajeASuscriptor(conexionGamecard, IDMensajeGet, GET, getPokemon))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %ld", argv[1], argv[2], argv[3], IDMensajeGet);
						exit(-1);
					}

					liberar_conexion_cliente(conexionGamecard);

					free(getPokemon);

					break;

				case NEW:;
					NewPokemon* newPokemon = getNewPokemon(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
					long IDMensajeNew = (long)atoi(argv[7]);

					if(!enviarMensajeASuscriptor(conexionGamecard, IDMensajeNew, NEW, newPokemon))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %d %d %d %ld", argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), IDMensajeNew);
						exit(-1);
					}

					Ack* respuesta;

					recibirAck(conexionGamecard, &respuesta);

					liberar_conexion_cliente(conexionGamecard);

					free(newPokemon);

					break;

				default:;
					log_info(logger, "ERROR - No se reconoce el tipo de cola ingresada.");
					exit(-1);
			}
		break;
		}

	case SUSCRIPTOR_GAMEBOY:;
		TipoCola colaDeMensaje;
		pthread_t threadEscuchaBroker;
		ArgumentosEscucharBroker argumentos;

		if((colaDeMensaje = argvToTipoCola(argv[2])) == -1)
		{
			log_info(logger, "ERROR - No se reconoce el tipo de cola ingresada.");
			exit(-1);
		}

		ipBroker = config_get_string_value(config, "IP_BROKER");
		puertoBroker = config_get_string_value(config, "PUERTO_BROKER");

		conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);
		if(conexionBroker == 0)
		{
			log_info(logger, "ERROR - No se pudo crear la conexión con BROKER");
			exit(-1);
		}

		//Log obligatorio
		log_info(logger, "Conexión al proceso %s", argv[1]);

		int tiempo = atoi(argv[3]);
		time_t start = time(0);

		int enviado = enviarSuscripcion(conexionBroker, GAMEBOY, 1, colaDeMensaje);

		if(enviado)
		{
			//Log obligatorio
			log_info(logger, "Suscripción a la cola %s.", tipoColaToString(colaDeMensaje));
		} else {
			log_info(logger, "Error intentando suscribirse a la cola.");
		}

		argumentos.ip = ipBroker;
		argumentos.puerto = puertoBroker;
		argumentos.logger = logger;
		argumentos.socket = conexionBroker;

		pthread_create(&threadEscuchaBroker, NULL, (void*)escucharBroker, &argumentos);
		pthread_detach(threadEscuchaBroker);

		while ((time(0) - start) <= tiempo);

		pthread_cancel(threadEscuchaBroker);

		break;

	default:
		log_info(logger, "ERROR - No se reconoce el tipo de mensaje ingresado.");
		exit(-1);
	}

	terminar_programa(logger, config);

	return EXIT_SUCCESS;
}

void escucharBroker(ArgumentosEscucharBroker* argumentos)
{
	while(1)
	{
		OpCode codigo;
		int recibido;

		recibido = recv(argumentos->socket, &codigo, sizeof(OpCode), MSG_WAITALL);

		if(recibido == -1 || recibido == 0){
			exit(-1);
		}

		if(codigo != NUEVO_MENSAJE_SUSCRIBER)
		{
			exit(-1);
		}

		MensajeParaSuscriptor* mensaje = NULL;
		int recepcionExitosa = recibirMensajeSuscriber(argumentos->socket, argumentos->logger, GAMEBOY, &mensaje, argumentos->ip, argumentos->puerto);

		if(recepcionExitosa)
		{
			//Log obligatorio
			log_info(argumentos->logger, "Llegó nuevo mensaje de cola %s.", tipoColaToString(mensaje->cola));
		} else {
			log_info(argumentos->logger, "Error enviando mensaje.");
		}

		free(mensaje);
	}
}
