// proyecto Gameboy
// á: 160 é: 130 í: 161 ó: 162 ú: 163 Á: 181 É: 144 Í: 214 Ó: 224 Ú: 23 ñ: 164 Ñ: 165

#include <shared/utils.h>
#include <shared/messages.h>
#include <shared/structs.h>
#include <string.h>

int main(int argc, char **argv){
	
	//Simulamos el ingreso de los argumentos por consola a modo de prueba.
	//Eliminar luego de testear.
	/*
	argv[1] = "BROKER";
	argv[2] = "NEW_POKEMON";
	argv[3] = "Pikachu";
	argv[4] = "5";
	argv[5] = "10";
	argv[6] = "2";
	
	argv[1] = "TEAM";
	argv[2] = "APPEARED_POKEMON";
	argv[3] = "Pikachu";
	argv[4] = "5";
	argv[5] = "10";
	argv[6] = "1";

	argc = 7;
	argv[1] = "SUSCRIPTOR";
	argv[2] = "NEW_POKEMON";
	argv[3] = "10";

	argc = 4;
	*/


	int const cantidadMinArgc = 4, cantidadMaxArgc = 20;

	int conexionBroker, conexionTeam, conexionGamecard;
	char *ipBroker, *ipTeam, *ipGamecard;
	char *puertoBroker, *puertoTeam, *puertoGamecard;


	t_log* logger;
	t_config* config;

	logger = iniciar_logger("loggerGameBoy.log", "GameBoy");
	config = leer_config("configGameBoy.config", logger);

    printf("El nombre del programa es: %s\n",argv[0]);
    if(argc == 1){
        printf("\nNo se ha ingresado ningún argumento de línea de comando adicional que no sea el nombre del programa\n");
		printf("Formato de ejecución: ./gameboy [BROKER|GAMECARD|TEAM|SUSCRIPTOR] [NEW_POKEMON|APPEARED_POKEMON|CATCH_POKEMON|CAUGHT_POKEMON|GET_POKEMON] [ARGUMENTOS]*\n");
		abort();
	}
    if(argc >= cantidadMinArgc && argc <= cantidadMaxArgc)
    {
        printf("\nNumero de argumentos ingresados: %d",argc);
        printf("\n----Los siguientes son los argumentos de la línea de comando ingresados----");
        for(int counter = 0;counter<argc;counter++)
            printf("\nargv[%d]: %s",counter,argv[counter]);
	}
	else {
		printf("\nPor favor, revise que está ingresando como MINIMO %d argumentos, y como MAXIMO %d argumentos, y vuelva a intentar\n", cantidadMinArgc, cantidadMaxArgc);
		abort();
	}
printf("\n\n");


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

					if(!enviarPublisherSinIDCorrelativo(conexionBroker, GAMEBOY, pokemon, NEW))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
						abort();
					}

					log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);

					OpCode code;

					recv(conexionBroker, &code, sizeof(OpCode), 0);

					if(code == ID_MENSAJE){
						IDMensajePublisher* mensaje = malloc(sizeof(IDMensajePublisher));
						recibirIDMensajePublisher(conexionBroker, mensaje);
						log_info(logger, "EL ID ES %ld", mensaje->IDMensaje);
						log_info(logger, "LA COLA ES %s", tipoColaToString(mensaje->cola));

					}else {
						log_info(logger, "TODO MAL. RECIBÍ OTRO CODE.");
					}

					liberar_conexion_cliente(conexionBroker);

					free(pokemon);

					break;
				}

				case APPEARED:
				{
					AppearedPokemon* pokemon = getAppearedPokemon(argv[3], atoi(argv[4]), atoi(argv[5]));//usa malloc, entonces hay que hacer un free

					if(!enviarPublisherSinIDCorrelativo(conexionBroker, GAMEBOY, pokemon, APPEARED))
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

					if(!enviarPublisherSinIDCorrelativo(conexionBroker, GAMEBOY, pokemon, CATCH))
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

					if(!enviarPublisherSinIDCorrelativo(conexionBroker, GAMEBOY, pokemon, CAUGHT))
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

					if(!enviarPublisherSinIDCorrelativo(conexionBroker, GAMEBOY, pokemon, GET))
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
			ipTeam = config_get_string_value(config, "IP_TEAM");
			puertoTeam = config_get_string_value(config, "PUERTO_TEAM");
			log_info(logger, "IP_TEAM     %s y PUERTO_TEAM     %s", ipTeam, puertoTeam);
			conexionTeam = crear_conexion_cliente(ipTeam, puertoTeam);

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

					if(!enviarPublisherSinIDCorrelativo(conexionTeam, GAMEBOY, pokemon, APPEARED))
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
					long IDMensaje = 12345;
					if(!enviarMensajeASuscriptor(conexionGamecard, IDMensaje, 0, CATCH, pokemon))
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
					log_info(logger, "ENTRÓ A GET, manda a %s", pokemon->nombre);

					long IDMensaje = 12345;

					if(!enviarMensajeASuscriptor(conexionGamecard, IDMensaje, 0, GET, pokemon))
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

					long IDMensaje = 12345;

					if(!enviarMensajeASuscriptor(conexionGamecard, IDMensaje, 0, NEW, pokemon))
					{
						log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s", argv[1], argv[2], argv[3]);
						abort();
					}

					log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);

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
		log_info(logger, "IP_BROKER   %s y PUERTO_BROKER   %s", ipBroker, puertoBroker);

		conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);
		if(conexionBroker == 0)
		{
			log_info(logger, "ERROR - No se pudo crear la conexión con BROKER");
			abort();
		}

		log_info(logger, "OK - Se estableció correctamente la conexión con el BROKER");

		if((colaDeMensaje = argvToTipoCola(argv[2])) == -1)
		{
			log_info(logger, "ERROR - No se reconoce el tipo de cola ingresada.");
			abort();
		}

		int tiempo = atoi(argv[3]);
		time_t start = time(0);

		while ((time(0) - start) <= tiempo )
		{

			enviarSuscripcion(conexionBroker, GAMEBOY, 1, colaDeMensaje);

			//recibir mensaje
			char *mensaje = recibirMensaje(conexionBroker);

			//loguear mensaje recibido
			log_info(logger, "El mensaje recibido de la cola de mensajes %s es: %s\n", tipoColaToString(colaDeMensaje), mensaje);
		}

		break;
	}

		default:
		{
			log_info(logger, "ERROR - No se reconoce el tipo de mensaje ingresada.");
			abort();
		}
	}


	//enviar_mensaje

	//NewPokemon = {"Pikachu",5,10,2};

	//AppearedPokemon = {"Pikachu",1,5};

	//CatchPokemon = {"Pikachu",1,5};

	//CaughtPokemon = {0};

	//GetPokemon = {"Pikachu"};

	terminar_programa(logger, config);

	return EXIT_SUCCESS;
}
