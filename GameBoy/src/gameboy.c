// proyecto Gameboy

#include <shared/utils.h>
#include <shared/messages.h>
#include <shared/structs.h>


int main(int argc, char **argv){
	
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
		exit(1);
	}
    if(argc >= cantidadMinArgc && argc <= cantidadMaxArgc)
    {
        printf("\nNumero de argumentos ingresados: %d",argc);
        printf("\n----Los siguientes son los argumentos de la línea de comando ingresados----");
        for(int counter = 0;counter<argc;counter++)
            printf("\nargv[%d]: %s",counter,argv[counter]);
	}
	else {
		printf("\nPor favor, revise que esta ingresando como MINIMO %d argumentos, y como MAXIMO %d argumentos, y vuelva a intentar\n", cantidadMinArgc, cantidadMaxArgc);
		exit(3);
	}

printf("\n\n");

if (strcmp(argv[1],"BROKER") == 0){
	ipBroker = config_get_string_value(config, "IP_BROKER");
	puertoBroker = config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger, "IP_BROKER   %s y PUERTO_BROKER   %s", ipBroker, puertoBroker);
	conexionBroker = crear_conexion_cliente(ipBroker, puertoBroker);

	if (strcmp(argv[2],"NEW_POKEMON") == 0){
		NewPokemon* pokemon = malloc(sizeof(NewPokemon));
		pokemon->nombre = argv[3];
		pokemon->largoNombre = strlen(pokemon->nombre);
		pokemon->posX = atoi(argv[4]);
		pokemon->posY = atoi(argv[5]);
		pokemon->cantidad = atoi(argv[6]);

		int tamanio = sizeof(uint32_t) * 4 + pokemon->largoNombre + sizeof(TipoCola);

		if(enviarMensaje(pokemon, tamanio, PUBLISHER, NEW, conexionBroker) == -1){
			log_info(logger, "ERROR enviando el mensaje %s %s %s %d %d %d\n", argv[1], argv[2], pokemon->nombre, pokemon->posX, pokemon->posY, pokemon->cantidad);
			exit(-1);
		}

		log_info(logger, "El mensaje enviado es: %s %s %s %d %d %d\n", argv[1], argv[2], pokemon->nombre, pokemon->posX, pokemon->posY, pokemon->cantidad);

		OpCode code;
		int idMensaje;
		TipoCola cola;

		recv(conexionBroker, &code, sizeof(OpCode), 0);

		if(code == ID_MENSAJE){
			log_info(logger, "RECIBÍ UN ID");
			recv(conexionBroker, &idMensaje, sizeof(long), 0);
			log_info(logger, "EL ID ES %d", idMensaje);
			recv(conexionBroker, &cola, sizeof(TipoCola), 0);
			log_info(logger, "LA COLA ES %d", tipoColaToString(cola));
		}else {
			log_info(logger, "TODO MAL. RECIBÍ OTRO CODE.");
		}


		liberar_conexion_cliente(conexionBroker);

		free(pokemon);
	}

	else if (strcmp(argv[2],"APPEARED_POKEMON") == 0){
			AppearedPokemon* pokemon = malloc(sizeof(AppearedPokemon));
			pokemon->nombre = argv[3];
			pokemon->largoNombre = strlen(pokemon->nombre);
			pokemon->posX = atoi(argv[4]);
			pokemon->posY = atoi(argv[5]);

			int tamanio = sizeof(uint32_t) * 3 + pokemon->largoNombre + sizeof(TipoCola);

			if(enviarMensaje(pokemon, tamanio, PUBLISHER, APPEARED, conexionBroker) == -1){
				log_info(logger, "ERROR enviando el mensaje %s %s %s %d %d\n", argv[1], argv[2], pokemon->nombre, pokemon->posX, pokemon->posY);
				exit(-1);
			}

			log_info(logger, "El mensaje enviado es: %s %s %s %d %d\n", argv[1], argv[2], pokemon->nombre, pokemon->posX, pokemon->posY);

			liberar_conexion_cliente(conexionBroker);

			free(pokemon);
		}

	else if (strcmp(argv[2],"CATCH_POKEMON") == 0){
				CatchPokemon* pokemon = malloc(sizeof(CatchPokemon));
				pokemon->nombre = argv[3];
				pokemon->largoNombre = strlen(pokemon->nombre);
				pokemon->posX = atoi(argv[4]);
				pokemon->posY = atoi(argv[5]);

				int tamanio = sizeof(uint32_t) * 3 + pokemon->largoNombre + sizeof(TipoCola);

				if(enviarMensaje(pokemon, tamanio, PUBLISHER, CATCH, conexionBroker) == -1){
					log_info(logger, "ERROR enviando el mensaje %s %s %s %d %d\n", argv[1], argv[2], pokemon->nombre, pokemon->posX, pokemon->posY);
					exit(-1);
				}

				log_info(logger, "El mensaje enviado es: %s %s %s %d %d\n", argv[1], argv[2], pokemon->nombre, pokemon->posX, pokemon->posY);

				liberar_conexion_cliente(conexionBroker);

				free(pokemon);
			}

	else if (strcmp(argv[2],"CAUGHT_POKEMON") == 0){
				CaughtPokemon* pokemon = malloc(sizeof(CaughtPokemon));
				pokemon->loAtrapo = atoi(argv[3]); //si se pudo o no atrapar al pokemon (0 o 1)

				int tamanio = sizeof(uint32_t) + sizeof(TipoCola);

				if(enviarMensaje(pokemon, tamanio, PUBLISHER, CAUGHT, conexionBroker) == -1 ){
					log_info(logger, "ERROR enviando el mensaje %s %s %d\n", argv[1], argv[2], pokemon->loAtrapo);
					exit(-1);
				}

				log_info(logger, "El mensaje enviado es: %s %s %d\n", argv[1], argv[2], pokemon->loAtrapo);

				liberar_conexion_cliente(conexionBroker);

				free(pokemon);
			}

	else if (strcmp(argv[2],"GET_POKEMON") == 0){
				GetPokemon* pokemon = malloc(sizeof(GetPokemon));
				pokemon->nombre = argv[3];
				pokemon->largoNombre = strlen(pokemon->nombre);

				int tamanio = sizeof(uint32_t) + pokemon->largoNombre + sizeof(TipoCola);

				if(enviarMensaje(pokemon, tamanio, PUBLISHER, GET, conexionBroker) == -1){
					log_info(logger, "ERROR enviando el mensaje %s %s %s\n", argv[1], argv[2], pokemon->nombre);
					exit(-1);
				}

				log_info(logger, "El mensaje enviado es: %s %s %s\n", argv[1], argv[2], pokemon->nombre);

				liberar_conexion_cliente(conexionBroker);

				free(pokemon);
			}
}
else if (strcmp(argv[1],"TEAM") == 0){
		ipTeam = config_get_string_value(config, "IP_TEAM");
		puertoTeam = config_get_string_value(config, "PUERTO_TEAM");
		log_info(logger, "IP_TEAM     %s y PUERTO_TEAM     %s", ipTeam, puertoTeam);
		conexionTeam = crear_conexion_cliente(ipTeam, puertoTeam);

		if (strcmp(argv[2],"APPEARED_POKEMON") == 0){
			AppearedPokemon* pokemon = malloc(sizeof(AppearedPokemon));
			pokemon->nombre = argv[3];
			pokemon->largoNombre = strlen(pokemon->nombre);
			pokemon->posX = atoi(argv[4]);
			pokemon->posY = atoi(argv[5]);

			int tamanio = sizeof(uint32_t) * 3 + pokemon->largoNombre + sizeof(TipoCola);

			if(enviarMensaje(pokemon, tamanio, PUBLISHER, APPEARED, conexionTeam) == -1){
				log_info(logger, "ERROR enviando el mensaje %s %s %s %d %d\n", argv[1], argv[2], pokemon->nombre, pokemon->posX, pokemon->posY);
				exit(-1);
			}

			log_info(logger, "El mensaje enviado es: %s %s %s %d %d\n", argv[1], argv[2], pokemon->nombre, pokemon->posX, pokemon->posY);

			liberar_conexion_cliente(conexionTeam);

			free(pokemon);

		}
}
else if (strcmp(argv[1],"GAMECARD") == 0){
		ipGamecard = config_get_string_value(config, "IP_GAMECARD");
		puertoGamecard = config_get_string_value(config, "PUERTO_GAMECARD");
		log_info(logger, "IP_GAMECARD %s y PUERTO_GAMECARD %s", ipGamecard, puertoGamecard);
		conexionGamecard = crear_conexion_cliente(ipGamecard, puertoGamecard);

		if (strcmp(argv[2],"CATCH_POKEMON") == 0){
			CatchPokemon* pokemon = malloc(sizeof(CatchPokemon));
			pokemon->nombre = argv[3];
			pokemon->largoNombre = strlen(pokemon->nombre);
			pokemon->posX = atoi(argv[4]);
			pokemon->posY = atoi(argv[5]);

			int tamanio = sizeof(uint32_t) * 3 + pokemon->largoNombre + sizeof(TipoCola);

			if(enviarMensaje(pokemon, tamanio, PUBLISHER, CATCH, conexionGamecard) == -1){
				log_info(logger, "ERROR enviando el mensaje %s %s %s %d %d\n", argv[1], argv[2], pokemon->nombre, pokemon->posX, pokemon->posY);
				exit(-1);
			}

			log_info(logger, "El mensaje enviado es: %s %s %s %d %d\n", argv[1], argv[2], pokemon->nombre, pokemon->posX, pokemon->posY);

			liberar_conexion_cliente(conexionGamecard);

			free(pokemon);
		}

		else if (strcmp(argv[2],"GET_POKEMON") == 0){
				GetPokemon* pokemon = malloc(sizeof(GetPokemon));
				pokemon->nombre = argv[3];
				pokemon->largoNombre = strlen(pokemon->nombre);

				int tamanio = sizeof(uint32_t) + pokemon->largoNombre + sizeof(TipoCola);

				if(enviarMensaje(pokemon, tamanio, PUBLISHER, GET, conexionGamecard) == -1){
					log_info(logger, "ERROR enviando el mensaje %s %s %s\n", argv[1], argv[2], pokemon->nombre);
					exit(-1);
				}

				log_info(logger, "El mensaje enviado es: %s %s %s\n", argv[1], argv[2], pokemon->nombre);

				liberar_conexion_cliente(conexionGamecard);

				free(pokemon);
			}

}
else if (strcmp(argv[1],"SUSCRIPTOR") == 0){
		printf("./gameboy SUSCRIPTOR [COLA_DE_MENSAJES] [TIEMPO]\n");

		//mandarSuscripcion(conexionBroker, 1, stringToCola(argv[2]));
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
