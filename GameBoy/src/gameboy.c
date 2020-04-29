#include <commons/log.h>
#include <shared/utils.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	int prueba = crear_conexion("127.0.0.1", "4444");
	t_log* logger = log_create("testGameBoy.log", "GAMEBOY", true, LOG_LEVEL_TRACE);
	log_trace(logger, "Conexión dio %d", prueba);
	log_destroy(logger);
	return EXIT_SUCCESS;
}
