#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <pthread.h>

#include "socket.h"

#define LG_BUFFER 1024

void *traite_connexion		(void * sock);
int serveur_tcp				(const short port);
int quitter_le_serveur		(void);

//
pthread_mutex_t mutex;
int clients_socket_list[10] ; 
int nb_clients = 0;


// Serveur
int main () {
	return serveur_tcp(1884);
}


void send_message_to_all (char *msg, int sender) {
	pthread_mutex_lock(&mutex);

	for (int i=0; i<nb_clients; i++) {
		if (clients_socket_list[i] != sender)
			write(clients_socket_list[i], "Bonjour", strlen("Bonjour"));
	}
	
	pthread_mutex_unlock(&mutex);
}

void *traite_connexion (void *t_sock) {
	int sock = *(int *)t_sock;
	char buffer[LG_BUFFER];
	int	len_in;
	// Écanges serveur <=> client
	while ((len_in = read(sock, buffer, LG_BUFFER)) > 0) {
		if (!strcmp(buffer, "coucou")) {
			send_message_to_all(buffer, sock);
		}
		write(STDOUT_FILENO, buffer, len_in);
		printf("\n");
		//write(sock, buffer, len_in);
	}
	if (len_in < 0) {
		perror("read");
		exit(EXIT_FAILURE);
	}
	close(sock);

	// TODO: traiter les clients qui quittent
	//
	// TODO: regler le broblème de double bonjour
	return NULL;
}

int serveur_tcp (const short port) {
	int main_socket;
	int sub_socket;
	struct sockaddr_in address;
	pthread_t recvt;

	if ((main_socket = create_main_socket(port )) < 0) {
		perror("socket create");
		return -1;
	}
	fprintf(stdout, "Mon adresse >> ");
	affiche_adresse_socket(main_socket);

	while (! quitter_le_serveur()) {
		// Crée un nouveau socket pour le premier client de la liste d'attente
		socklen_t saddr_len = sizeof(struct sockaddr_in);
		if ((sub_socket = accept(main_socket, (struct sockaddr *) &address, &saddr_len)) < 0) {
			perror("accept");
			return -1;
		}

		// 
		pthread_mutex_lock(&mutex);

		// Ajoute le nouveau socket à la liste des sockets clients
		clients_socket_list[nb_clients++] = sub_socket;

		int *t_socket = malloc(sizeof(int *));
		*t_socket = sub_socket;

		// creating a thread for each client 
		pthread_create(&recvt, NULL, traite_connexion, (void *) t_socket);

		pthread_mutex_unlock(&mutex);
	}
	return 0;
}

// Permet de gérer Ctrl+C pour quitter
int quitter_le_serveur (void) {
	return 0;
}
