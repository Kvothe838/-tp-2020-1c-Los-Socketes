#include <shared/utils.h>
#include "brokerColas.h"
#include "conexionBroker.h"
#include "cache/genericCache.h"
#include <commons/process.h>

static void	sig_usr(int);
static void sig_usr2(int);

void err_sys(char * msg) {
  printf("%s \n", msg);
  exit(-1);
}

pthread_t threadIniciarServidor, threadEnviarMensajesSuscriptores;

int main(void) {
	t_config* config;
	IniciarServidorArgs argumentos;

	loggerObligatorio = iniciar_logger("broker_logger_obligatorio.log", "Broker");
	loggerInterno = iniciarLoggerSinConsola("broker_logger_interno.log", "Broker");
	config = leer_config("configBroker.config", loggerInterno);

	argumentos.ip = config_get_string_value(config, "IP_BROKER");
	argumentos.puerto = config_get_string_value(config, "PUERTO_BROKER");

	inicializarDataBasica(config);
	crearDiccionario();

	log_info(loggerObligatorio, "ID del proceso Broker: %d", process_getpid());

	//Forma de llamar al signal: kill -SIGUSR1 [ID]

	if(signal(SIGUSR1, sig_usr) == SIG_ERR)
		err_sys("can't catch SIGUSR1");
	if(signal(SIGUSR2, sig_usr2) == SIG_ERR)
		err_sys("can't catch SIGUSR2");
	/*Esta signal SIGUSR2 sirve sólo para terminar el programa de forma correcta, pasando por las liberaciones de
	 memoria del final del main.*/

	semColaMensajes = malloc(sizeof(sem_t));
	sem_init(semColaMensajes, 0, 0);
	pthread_mutex_init(&mutexGeneracionHash, NULL);
	ultimoIDMensaje = 0;

	pthread_create(&threadIniciarServidor, NULL,(void*)iniciarServidor, (void*)&argumentos);
	pthread_create(&threadEnviarMensajesSuscriptores, NULL,(void*)enviarMensajesSuscriptores, NULL);
	pthread_join(threadIniciarServidor, NULL);
	pthread_join(threadEnviarMensajesSuscriptores, NULL);

	liberarDiccionario();
	liberarCacheGeneric();
	sem_destroy(semColaMensajes);
	free(semColaMensajes);
	log_destroy(loggerObligatorio);
	terminar_programa(loggerInterno, config);

	return 0;
}

static void sig_usr(int signo)
{
	if(signo == SIGUSR1)
	{
		obtenerDumpGeneric();
	}
}

static void sig_usr2(int signo){
	if(signo == SIGUSR2)
	{
		pthread_cancel(threadIniciarServidor);
		pthread_cancel(threadEnviarMensajesSuscriptores);
	}

}
