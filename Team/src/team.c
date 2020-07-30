#include "team.h"

int main(void) {
	Config* configTeam = malloc(sizeof(Config));
	t_log* logger;
	logger = iniciar_logger("Team.log", "Team");
	cargarConfig(configTeam, logger);

	//Ejemplo de conexión a Broker para escuchar globalmente en APPEARED, LOCALIZED y CAUGHT.

	int conexionBroker = crear_conexion_cliente(configTeam->ip, configTeam->puerto);

	/*Si bien el segundo parámetro de enviarSuscripcion es de tipo TipoModulo, en el caso particular del Team va a enviar
	 el ID del Team, que en la configuración siempre va a empezar por 1, pero al momento de enviarlo siempre va a empezar
	 por 5. Esto se puede porque TipoModulo es otra forma de llamar al int.*/
	int suscripcionEnviada = enviarSuscripcion(conexionBroker, configTeam->ID, 3, APPEARED, LOCALIZED, CAUGHT);

	/*Esta lista va a contener todos los IDs de mensjes que espere respuesta.
	 La lista se va a llenar cuando se envíe un nuevo mensaje por alguna cola, y se reciba el ID correspondiente.
	 La lista se va a leer cuando se reciba un nuevo mensaje de una cola y se filtre por IDCorrelativo.*/
	//t_list* IDsCorrelativos = list_create();

	if(suscripcionEnviada)
	{
		while(1){
			OpCode codigo;
			recv(conexionBroker, &codigo, sizeof(OpCode), 0);
			if(codigo == NUEVO_MENSAJE_SUSCRIBER){
				MensajeParaSuscriptor* mensaje= NULL;
				int recepcionExitosa = recibirMensajeSuscriber(conexionBroker, logger, TEAM, &mensaje, configTeam->ip, configTeam->puerto);
				log_info(logger, "ID CORRELATIVO: %d", mensaje->IDMensajeCorrelativo);
				if(recepcionExitosa)
				{
					/*Acá trabajás el mensaje como más te guste. Podés filtrar primero por id correlativo,
					o directamente trabajar con la cola. No se puede generalizar en el Broker esto porque siempre
					vas a necesitar hacer un if de la cola para saber cómo castear el dato.*/

					//Tiro ejemplo de filtrar sólo por la cola:

					if(mensaje->cola == APPEARED)
					{
						AppearedPokemon* pokemon = deserializarAppeared(mensaje->contenido);

						//Hacés lo que te plazca con el pokemon.
						log_info(logger, "Llegó Appeared con nombre: %s", pokemon->nombre);

						free(pokemon);
					}

					if(mensaje->cola == LOCALIZED)
					{
						LocalizedPokemon* pokemon = deserializarLocalized(mensaje->contenido);

						log_info(logger, "Llegó Localized con nombre: %s y cantidad de pares: %d", pokemon->nombre, pokemon->cantidadDeParesDePosiciones);

						free(pokemon);
					}

					//Tiro ejemplo para filtrar por IDCorrelativo.

					/*for(int i = 0; i < list_size(IDsCorrelativos); i++)
					{
						long* IDCorrelativoAEsperar = list_get(IDsCorrelativos, i);

						if(mensaje->IDMensajeCorrelativo == *IDCorrelativoAEsperar)
						{
							log_info(logger, "Me llegó el puto mensaje que esperaba, respuesta al ID %d", *IDCorrelativoAEsperar);
						}
					}*/

					free(mensaje);
				}
			}
		}
	}

	//Ejemplo de conexión a Broker para enviar un mensaje a la cola GET.

	/*GetPokemon* pokemon = getGetPokemon("Charmander");

	if(enviarPublisherSinIDCorrelativo(conexionBroker, TEAM, pokemon, GET))
	{
		log_info(logger, "ERROR - No se pudo enviar el mensaje");
	}

	OpCode codigoOperacion;

	recv(conexionBroker, &codigoOperacion, sizeof(OpCode), 0);

	if(codigoOperacion == ID_MENSAJE){
		IDMensajePublisher* mensaje = NULL;
		int recibidoExitoso = recibirIDMensajePublisher(conexionBroker, mensaje);

		if(recibidoExitoso)
		{
			list_add(IDsCorrelativos, &(mensaje->IDMensaje));
			free(mensaje);
		}
	}*/

	//Fin de ejemplos.

	Team team = inicializarTeam(configTeam->posiciones,configTeam->pertenecientes,configTeam->objetivos);
	planificacion_fifo();
	free(configTeam);
	liberarMemoria(team);
	printf(" \nprograma finalizado");
	return 0;
}















