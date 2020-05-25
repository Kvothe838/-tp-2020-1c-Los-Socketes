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

	logger = iniciar_logger("GameBoy.log", "GAMEBOY");
	config = leer_config("GameBoy.config", logger);


    printf("El nombre del programa es: %s\n",argv[0]); 
    if(argc == 1){
        printf("\nNo se ha ingresado ningún argumento de línea de comando adicional que no sea el nombre del programa\n");
		printf("Formato de ejecución: ./gameboy [BROKER|GAMECARD|TEAM|SUSCRIPTOR] [NEW_POKEMON|APPEARED_POKEMON|CATCH_POKEMON|CAUGHT_POKEMON|GET_POKEMON] [ARGUMENTOS]*\n");
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
						NewPokemon pokemon;
						pokemon.nombre = argv[3];
						pokemon.largoNombre = strlen(pokemon.nombre);
						pokemon.posX = atoi(argv[4]);
						pokemon.posY = atoi(argv[5]);
						pokemon.cantidad = atoi(argv[6]);
						
						Buffer* buffer = malloc(sizeof(Buffer));

						buffer->size = sizeof(uint32_t) * 4 + strlen(pokemon.nombre) + 1;

						void* stream = malloc(buffer->size);
						int offset = 0;

						memcpy(stream + offset, &pokemon.posX, sizeof(uint32_t));
						offset += sizeof(uint32_t);

						memcpy(stream + offset, &pokemon.posY, sizeof(uint32_t));
						offset += sizeof(uint32_t);

						memcpy(stream + offset, &pokemon.cantidad, sizeof(uint32_t));
						offset += sizeof(uint32_t);

						memcpy(stream + offset, &pokemon.largoNombre, sizeof(uint32_t));
						offset += sizeof(uint32_t);

						memcpy(stream + offset, pokemon.nombre, strlen(pokemon.nombre) + 1);

						buffer->stream = stream;

						//Entonces podemos llenar el paquete con el buffer///

						Paquete* paquete = malloc(sizeof(paquete));

						paquete->codigoOperacion = NEW;
						paquete->buffer = buffer;

						//Armamos el stream a enviar//
						void* aEnviar = malloc(buffer->size + sizeof(TipoCola) + sizeof(uint32_t));
						int offsetS = 0;

						memcpy(aEnviar + offsetS, &(paquete->codigoOperacion), sizeof(TipoCola));
						offsetS += sizeof(TipoCola);

						memcpy(aEnviar + offsetS, &(paquete->buffer->size), sizeof(uint32_t));
						offsetS += sizeof(uint32_t);

						memcpy(aEnviar + offsetS, paquete->buffer->stream, paquete->buffer->size);

						//Enviamos//
						send(conexionBroker, aEnviar, buffer->size + sizeof(TipoCola) + sizeof(uint32_t), 0);

						log_info(logger, "El mensaje enviado es: %s %s %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
						
						liberar_conexion_cliente(conexionBroker);

						free(aEnviar);
						free(paquete->buffer->stream);
						free(paquete->buffer);
						free(paquete);
					}
}
else if (strcmp(argv[1],"TEAM") == 0){
						ipTeam = config_get_string_value(config, "IP_TEAM");
						puertoTeam = config_get_string_value(config, "PUERTO_TEAM");
						log_info(logger, "IP_TEAM     %s y PUERTO_TEAM     %s", ipTeam, puertoTeam);
						conexionTeam = crear_conexion_cliente(ipTeam, puertoTeam);
}
else if (strcmp(argv[1],"GAMECARD") == 0){
						ipGamecard = config_get_string_value(config, "IP_GAMECARD");
						puertoGamecard = config_get_string_value(config, "PUERTO_GAMECARD");
						log_info(logger, "IP_GAMECARD %s y PUERTO_GAMECARD %s", ipGamecard, puertoGamecard);
						conexionGamecard = crear_conexion_cliente(ipGamecard, puertoGamecard);
}
else if (strcmp(argv[1],"SUSCRIPTOR") == 0){
						printf("./gameboy SUSCRIPTOR [COLA_DE_MENSAJES] [TIEMPO]\n");
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
