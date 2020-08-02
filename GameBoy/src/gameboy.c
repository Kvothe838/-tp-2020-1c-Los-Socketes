// proyecto Gameboy
// á: 160 é: 130 í: 161 ó: 162 ú: 163 Á: 181 É: 144 Í: 214 Ó: 224 Ú: 23 ñ: 164 Ñ: 165

#include <shared/utils.h>
#include <shared/messages.h>
#include <shared/structs.h>
#include <string.h>

int main(int argc, char **argv){
	int const cantidadMinArgc = 2, cantidadMaxArgc = 20;

	int conexionBroker, conexionGamecard, cantidadTeams;
	char *ipBroker, *ipGamecard, *puertoBroker, *puertoGamecard, **ipsPuertosTeam;

	t_log* logger;
	t_config* config;

	logger = iniciar_logger("loggerGameBoy.log", "GameBoy");
	config = leer_config("configGameBoy.config", logger);

    if(argc == 1){
        printf("\nNo se ha ingresado ningún argumento de línea de comando adicional que no sea el nombre del programa\n");
		printf("Formato de ejecución: ./gameboy [BROKER|GAMECARD|TEAM|SUSCRIPTOR] [NEW_POKEMON|APPEARED_POKEMON|CATCH_POKEMON|CAUGHT_POKEMON|GET_POKEMON] [ARGUMENTOS]*\n");
		abort();
	}

    if(argc < cantidadMinArgc || argc > cantidadMaxArgc)
    {
		printf("\nPor favor, revise que está ingresando como MINIMO %d argumentos, y como MAXIMO %d argumentos, y vuelva a intentar\n", cantidadMinArgc, cantidadMaxArgc);
		abort();
	}

	printf("\n\n");
	IDMensajePublisher* mensajeRecibido;

	switch(argvToModuloGameboy(argv[1]))
	{
		case BROKER_GAMEBOY:
		{
			ipBroker = config_get_string_value(config, "IP_BROKER");
			puertoBroker = config_get_string_value(config, "PUERTO_BROKER");
			log_info(logger, "IP_BROKER   %s y PUERTO_BROKER   %s", ipBroker, puertoBroker);
			conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);
			if(conexionBroker == 0){
				log_info(logger, "ERROR - No se pudo crear la conexión con %s", argv[1]);
				abort();
			}

			log_info(logger, "OK - Se estableció correctamente la conexión con el %s", argv[1]);

			switch(argvToTipoCola(argv[2]))
			{
				case NEW:
				{
					NewPokemon* pokemon = getNewPokemon(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6])); //usa malloc, entonces hay que hacer un free

					if(!enviarPublisherSinIDCorrelativo(logger, conexionBroker, GAMEBOY, pokemon, NEW, &mensajeRecibido))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
						abort();
					}

					liberar_conexion_cliente(conexionBroker);

					free(pokemon);

					break;
				}

				case APPEARED:
				{
					AppearedPokemon* pokemon = getAppearedPokemon(argv[3], atoi(argv[4]), atoi(argv[5]));//usa malloc, entonces hay que hacer un free

					if(!enviarPublisherConIDCorrelativo(logger, conexionBroker, GAMEBOY, pokemon, APPEARED, (int)argv[6], &mensajeRecibido))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);
						abort();
					}

					log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

					liberar_conexion_cliente(conexionBroker);

					free(pokemon);

					break;
				}

				case CATCH:
				{
					CatchPokemon* pokemon = getCatchPokemon(argv[3], atoi(argv[4]), atoi(argv[5]));//usa malloc, entonces hay que hacer un free

					if(!enviarPublisherSinIDCorrelativo(logger, conexionBroker, GAMEBOY, pokemon, CATCH, &mensajeRecibido))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %d %d", argv[1], argv[2], argv[3], argv[4], argv[5]);
						abort();
					}

					log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %s %d %d", argv[1], argv[2], argv[3], argv[4], argv[5]);

					liberar_conexion_cliente(conexionBroker);

					free(pokemon);

					break;
				}

				case CAUGHT:
				{
					CaughtPokemon* pokemon = getCaughtPokemon(atoi(argv[3]));//usa malloc, entonces hay que hacer un free

					if(!enviarPublisherSinIDCorrelativo(logger, conexionBroker, GAMEBOY, pokemon, CAUGHT, &mensajeRecibido))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %d", argv[1], argv[2], argv[3]);
						abort();
					}

					log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %d", argv[1], argv[2], argv[3]);

					liberar_conexion_cliente(conexionBroker);

					free(pokemon);

					break;
				}

				case GET:
				{
					GetPokemon* pokemon = getGetPokemon(argv[3]);//usa malloc, entonces hay que hacer un free

					if(!enviarPublisherSinIDCorrelativo(logger, conexionBroker, GAMEBOY, pokemon, GET, &mensajeRecibido))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s", argv[1], argv[2], argv[3]);
						abort();
					}

					log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %s", argv[1], argv[2], argv[3]);

					liberar_conexion_cliente(conexionBroker);

					free(pokemon);

					break;
				}

				default:
				{
					log_info(logger, "ERROR. No se reconoce el tipo de cola ingresada.");
					abort();
				}
			}
		break;
		}

		case TEAM_GAMEBOY:
		{
			ipsPuertosTeam = config_get_array_value(config, "IPS_PUERTOS_TEAM");
			cantidadTeams = config_get_int_value(config, "CANTIDAD_TEAMS");

			for(int i = 0; i < cantidadTeams; i = i+2){
				char* ipTeam = ipsPuertosTeam[i];
				char* puertoTeam = ipsPuertosTeam[i+1];

				int conexionTeam = crear_conexion_cliente(ipTeam, puertoTeam);

				if(conexionTeam == 0){
					log_info(logger, "ERROR - No se pudo crear la conexión con %s", argv[1]);
					abort();
				}

				log_info(logger, "OK - Se estableció correctamente la conexión con el %s", argv[1]);


				switch(argvToTipoCola(argv[2]))
				{
					case APPEARED:
					{
						AppearedPokemon* pokemon = getAppearedPokemon(argv[3], atoi(argv[4]), atoi(argv[5]));//usa malloc, entonces hay que hacer un free

						if(!enviarMensajeASuscriptor(conexionTeam, 1, APPEARED, pokemon))
						{
							log_info(logger, "ERROR. No se pudo enviar el mensaje %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);
							abort();
						}

						log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

						liberar_conexion_cliente(conexionTeam);

						free(pokemon);

						break;
					}

					default:
					{
						log_info(logger, "ERROR. No se reconoce el tipo de cola ingresada.");
						abort();
					}
				}
			}
		break;
		}

		case GAMECARD_GAMEBOY:
		{
			ipGamecard = config_get_string_value(config, "IP_GAMECARD");
			puertoGamecard = config_get_string_value(config, "PUERTO_GAMECARD");
			log_info(logger, "IP_GAMECARD %s y PUERTO_GAMECARD %s\n", ipGamecard, puertoGamecard);
			conexionGamecard = crear_conexion_cliente(ipGamecard, puertoGamecard);

			if(conexionGamecard == 0){
				log_info(logger, "ERROR - No se pudo crear la conexión con %s", argv[1]);
				abort();
			}

			log_info(logger, "OK - Se estableció correctamente la conexión con el %s", argv[1]);

			switch(argvToTipoCola(argv[2]))
			{
				case CATCH:
				{
					CatchPokemon* pokemon = getCatchPokemon(argv[3], atoi(argv[4]), atoi(argv[5]));//usa malloc, entonces hay que hacer un free
					long IDMensaje = (long)atoi(argv[6]);

					if(!enviarMensajeASuscriptor(conexionGamecard, IDMensaje, CATCH, pokemon))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);
						abort();
					}

					log_info(logger, "Largo del nombre %s, %d vs %d", pokemon->nombre, strlen(pokemon->nombre), pokemon->largoNombre);
					log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

					liberar_conexion_cliente(conexionGamecard);

					free(pokemon);

					break;
				}

				case GET:
				{
					GetPokemon* pokemon = getGetPokemon(argv[3]);//usa malloc, entonces hay que hacer un free
					long IDMensaje = (long)atoi(argv[4]);

					if(!enviarMensajeASuscriptor(conexionGamecard, IDMensaje, GET, pokemon))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s", argv[1], argv[2], argv[3]);
						abort();
					}

					log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %s", argv[1], argv[2], argv[3]);

					liberar_conexion_cliente(conexionGamecard);

					free(pokemon);

					break;
				}

				case NEW:
				{
					NewPokemon* pokemon = getNewPokemon(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
					long IDMensaje = (long)atoi(argv[7]);
					log_info(logger, "ID MENSAJE: %ld", IDMensaje);

					if(!enviarMensajeASuscriptor(conexionGamecard, IDMensaje, NEW, pokemon))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s", argv[1], argv[2], argv[3]);
						abort();
					}

					log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);

					Ack* respuesta;

					int recibidoExitoso = recibirAck(conexionGamecard, &respuesta);

					if(recibidoExitoso){
						log_info(logger, "Recibido ACK");
					} else {
						log_info(logger, "Error al recibir ACK");
					}

					liberar_conexion_cliente(conexionGamecard);

					free(pokemon);

					break;
				}


				default:
				{
					log_info(logger, "ERROR - No se reconoce el tipo de cola ingresada.");
					abort();
				}
			}
		break;
		}

	case SUSCRIPTOR_GAMEBOY:
	{
		// ./gameboy SUSCRIPTOR [COLA_DE_MENSAJES] [TIEMPO]
		//   argv[0]   argv[1]      argv[2]         argv[3]

		TipoCola colaDeMensaje;

		ipBroker = config_get_string_value(config, "IP_BROKER");
		puertoBroker = config_get_string_value(config, "PUERTO_BROKER");

		conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);
		if(conexionBroker == 0)
		{
			log_info(logger, "ERROR - No se pudo crear la conexión con BROKER");
			abort();
		}

		if((colaDeMensaje = argvToTipoCola(argv[2])) == -1)
		{
			log_info(logger, "ERROR - No se reconoce el tipo de cola ingresada.");
			abort();
		}

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

		while ((time(0) - start) <= tiempo )
		{
			OpCode codigo;
			int recibido;

			recibido = recv(conexionBroker, &codigo, sizeof(OpCode), MSG_WAITALL);

			if(recibido == -1 || recibido == 0){
				log_info(logger, "Error recibiendo código");
				abort();
			}

			if(codigo != NUEVO_MENSAJE_SUSCRIBER)
			{
				log_info(logger, "Recibí otro código");
				abort();
			}

			MensajeParaSuscriptor* mensaje = NULL;
			int recepcionExitosa = recibirMensajeSuscriber(conexionBroker, logger, GAMEBOY, &mensaje, ipBroker, puertoBroker);

			if(recepcionExitosa)
			{
				//Log obligatorio
				log_info(logger, "Llegó nuevo mensaje de cola %s.", tipoColaToString(mensaje->cola));
			} else {
				log_info(logger, "Error enviando mensaje.");
			}

			free(mensaje);
		}

		break;
	}

		default:
		{
			log_info(logger, "ERROR - No se reconoce el tipo de mensaje ingresado.");
			abort();
		}
	}

	terminar_programa(logger, config);

	return EXIT_SUCCESS;
}
