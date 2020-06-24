#ifndef SOCKET_H
#define SOCKET_H

int create_main_socket		(const short);
int affiche_adresse_socket	(int sock);

typedef struct client_t {
	int sock;
	int flux[100];
	int count_flux;
} client_t;

#endif
