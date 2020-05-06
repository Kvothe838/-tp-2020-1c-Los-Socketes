#ifndef UTILS_H_
#define UTILS_H_

#include "../shared/structs.h"
#include "../shared/connection.h"
#include "../shared/serialize.h"
#include "../shared/messages.h"

t_log* iniciar_logger(char* nombreLogger, char* nombrePrograma);
t_config* leer_config(char* nombreConfig, t_log* logger);
void terminar_programa(t_log* logger, t_config* config);

#endif
