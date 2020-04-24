#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

int main(void){
	printf("HOLA MUNDO");

	t_log* logger = log_create("broker.log", "Broker", true, LOG_LEVEL_INFO);

	log_info(logger, "Hola mundo");
	return 0;
}
