#include <stdio.h>
#include <unistd.h>

#include <stdlib.h> 
#include <string.h> 

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h> 

#include <sys/types.h>
#include <sys/socket.h>

#include <pthread.h>

#include "socket.h"

#define LG_BUFFER 1024

int serveur_tcp				(const short port);
int quitter_le_serveur		(void);
void *traite_connexion		(int sock);

//
pthread_mutex_t mutex;
int clients_socek_list[10] ; 
int clients_enabled[10] ;
int nb_clients = 0;


// Serveur
int main () {
	return serveur_tcp(1885);
}


void send_message_to_all (char *msg, int sender) {

	pthread_mutex_lock(&mutex);

	for (int i=0; i<nb_clients; i++) {
		if ((clients_socek_list[i] != sender)&&(clients_enabled[i] == 1))
			write(clients_socek_list[i], "Bonjour", strlen("Bonjour"));
	}
	
	pthread_mutex_unlock(&mutex);
}

void *traite_connexion (int sock) {
	char buffer[256];
	int	len_in;
	// Écanges serveur <=> client
	while ((len_in = read(sock, buffer, LG_BUFFER)) == 0) {
		/*
		if (len_in < 0) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		*/

		if (!strcmp(buffer, "coucou")) {
			send_message_to_all(buffer, sock);
		}
		//if (!strcmp(buffer, ""))
		//	continue;
		write(STDOUT_FILENO, buffer, len_in);
		write(sock, buffer, len_in);
		printf("\n");
	}
	close(sock);

	for (int i=0; i<nb_clients; i++) {
			if (clients_enabled[i] == sock)
				clients_enabled[i] = 0;
	}
	return 0;
}

int serveur_tcp (const short port) {
	int sock_contact;
	int nouveau_socket;
	struct sockaddr_in adresse;
	socklen_t longueur;

	pthread_t recvt;
	
	sock_contact = create_socket(NULL, port );
	if (sock_contact < 0)
		return -1;
	// Crée une liste d'attente 
	listen(sock_contact, 5);
	fprintf(stdout, "Mon adresse >> ");
	affiche_adresse_socket(sock_contact);

	while (! quitter_le_serveur()) {

		// Crée un nouveau socket pour le premier client de la liste d'attente
		longueur = sizeof(struct sockaddr_in);
		nouveau_socket = accept(sock_contact, (struct sockaddr *)& adresse, & longueur);
		if (nouveau_socket < 0) {
			perror("accept");
			return -1;
		}

		// 
		pthread_mutex_lock(&mutex);

		// Ajoute le nouveau socket à la liste des sockets clients
		clients_enabled[nb_clients] = 1 ;
		clients_socek_list[nb_clients++] = nouveau_socket;

		// creating a thread for each client 
		pthread_create(&recvt, NULL, (void *) traite_connexion, &nouveau_socket);

		pthread_mutex_unlock(&mutex);


		/*
		switch (fork()) {
			case 0 :
				close(sock_contact);
				traite_connexion(sock_connectee, clients, clients_enabled, nb_clients);
				exit(EXIT_SUCCESS);
			case -1:
				perror("fork");
				return -1;
			default :
				close(sock_connectee);
		}
		*/


	}
	return 0;
}

// Permet de gérer Ctrl+C pour quitter
int quitter_le_serveur (void) {
	return 0;
}
