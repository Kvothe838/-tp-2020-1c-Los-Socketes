#include "../shared/connection.h"

int crear_conexion_cliente(char *ip, char* puerto)
{
  printf("8.1\n");
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
  printf("8.2\n");
  fflush(stdout);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);
  printf("8.3\n");
  fflush(stdout);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

  printf("8.4: %d\nAI_ADDR: ", socket_cliente);
  fflush(stdout);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		socket_cliente = -1;
	}

  printf("8.5");
  fflush(stdout);

	freeaddrinfo(server_info);

  printf("8.6");
  fflush(stdout);

	return socket_cliente;
}

void liberar_conexion_cliente(int socket_cliente)
{
	close(socket_cliente);
}
