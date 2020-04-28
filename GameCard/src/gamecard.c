#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	int prueba = crear_conexion("189", "189");
	t_log* logger = log_create("testGameCard.log", "GAMECARD", true, LOG_LEVEL_TRACE);
	log_trace(logger, "Conexión dio %d", prueba);
	return EXIT_SUCCESS;
}

