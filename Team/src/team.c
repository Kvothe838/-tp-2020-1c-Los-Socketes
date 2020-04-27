#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include<commons/temporal.h>

int main(void){
	printf("Hora: ");
	char* tiempo = temporal_get_string_time();
	puts(tiempo);
	free(tiempo);

	t_log* logger = log_create("team.log", "Team", true, LOG_LEVEL_INFO);

	log_info(logger, "Hola mundo");
	return 0;
}
