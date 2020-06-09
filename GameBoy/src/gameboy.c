// proyecto Gameboy
// á: 160 é: 130 í: 161 ó: 162 ú: 163 Á: 181 É: 144 Í: 214 Ó: 224 Ú: 23 ñ: 164 Ñ: 165

#include <shared/utils.h>
#include <shared/messages.h>
#include <shared/structs.h>

int main(int argc, char **argv){
	
	//Para usar si se quiere probar rápido.
	/*argv[1] = "BROKER";
	argv[2] = "NEW_POKEMON";
	argv[3] = "Pikachu";
	argv[4] = "5";
	argv[5] = "10";
	argv[6] = "2";*/
	
	/*argv[1] = "TEAM";
	argv[2] = "APPEARED_POKEMON";
	argv[3] = "Pikachu";
	argv[4] = "5";
	argv[5] = "10";
	argv[6] = "1";*/

	argc = 7;

	int const cantidadMinArgc = 4, cantidadMaxArgc = 7;

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
		printf("Formato de ejecución: ./gameboy [BROKER|GAMECARD|TEAM] [NEW_POKEMON|APPEARED_POKEMON|CATCH_POKEMON|CAUGHT_POKEMON|GET_POKEMON] [ARGUMENTOS]*\n");
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


switch(argvToTipoModulo(argv[1]))
{
	case BROKER:
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

				int tamanio = sizeof(uint32_t) * 4 + pokemon->largoNombre + sizeof(TipoCola);

				if(enviarMensaje(pokemon, tamanio, PUBLISHER, NEW, conexionBroker) == -1)
				{
					log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
					abort();
				}

				log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);

				OpCode code;
				long idMensaje;
				TipoCola cola;
				int size;

				recv(conexionBroker, &code, sizeof(OpCode), 0);

				if(code == ID_MENSAJE){
					log_info(logger, "RECIBÍ ALGO");
					recv(conexionBroker, &size, sizeof(int), 0);
					log_info(logger, "EL SIZE ES %d", size);
					recv(conexionBroker, &idMensaje, sizeof(long), 0);
					log_info(logger, "EL ID ES %d", idMensaje);
					recv(conexionBroker, &cola, sizeof(TipoCola), 0);
					log_info(logger, "LA COLA ES %s", tipoColaToString(cola));
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

				int tamanio = sizeof(uint32_t) * 3 + pokemon->largoNombre + sizeof(TipoCola);

				if(enviarMensaje(pokemon, tamanio, PUBLISHER, APPEARED, conexionBroker) == -1)
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

				int tamanio = sizeof(uint32_t) * 3 + pokemon->largoNombre + sizeof(TipoCola);

				if(enviarMensaje(pokemon, tamanio, PUBLISHER, CATCH, conexionBroker) == -1)
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

				int tamanio = sizeof(uint32_t) + sizeof(TipoCola);

				if(enviarMensaje(pokemon, tamanio, PUBLISHER, CAUGHT, conexionBroker) == -1 )
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

				int tamanio = sizeof(uint32_t) + pokemon->largoNombre + sizeof(TipoCola);

				if(enviarMensaje(pokemon, tamanio, PUBLISHER, GET, conexionBroker) == -1)
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

	case TEAM:
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

				int tamanio = sizeof(uint32_t) * 3 + pokemon->largoNombre + sizeof(TipoCola);

				if(enviarMensaje(pokemon, tamanio, PUBLISHER, APPEARED, conexionTeam) == -1)
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

	case GAMECARD:
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

				int tamanio = sizeof(uint32_t) * 3 + pokemon->largoNombre + sizeof(TipoCola);

				if(enviarMensaje(pokemon, tamanio, PUBLISHER, CATCH, conexionGamecard) == -1)
				{
					log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);
					abort();
				}

				log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

				liberar_conexion_cliente(conexionGamecard);

				free(pokemon);

				break;
			}

			case GET:
			{
				GetPokemon* pokemon = getGetPokemon(argv[3]);//usa malloc, entonces hay que hacer un free

				int tamanio = sizeof(uint32_t) + pokemon->largoNombre + sizeof(TipoCola);

				if(enviarMensaje(pokemon, tamanio, PUBLISHER, GET, conexionGamecard) == -1)
				{
					log_info(logger, "ERROR - No se pudo enviar el mensaje: %s %s %s", argv[1], argv[2], argv[3]);
					abort();
				}

				log_info(logger, "OK - Se envió correctamente el mensaje: %s %s %s", argv[1], argv[2], argv[3]);

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

	case SUSCRIPTOR:
	{
		printf("./gameboy SUSCRIPTOR [COLA_DE_MENSAJES] [TIEMPO]\n");

		//mandarSuscripcion(conexionBroker, 1, stringToCola(argv[2]));
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
