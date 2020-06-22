#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <stdint.h>

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
client_t clients_list[10]; 
int nb_clients = 0;


// Serveur
int main () {
	return serveur_tcp(1884);
}


void send_message_to_all (char *msg, int sender) {
	pthread_mutex_lock(&mutex);

	for (int i=0; i<nb_clients; i++) {
		if (clients_list[i].sock != sender)
			write(clients_list[i].sock, "Bonjour", strlen("Bonjour"));
	}
	
	pthread_mutex_unlock(&mutex);
}

void send_data_to_subscribed (int id_flux, char *data) {
	printf("send: %d, %s\n", id_flux, data);
	pthread_mutex_lock(&mutex);

	for (int i=0; i<nb_clients; i++) {
		for (int j=0; j<clients_list[i].count_flux; j++) {
			if (clients_list[i].flux[j] == id_flux) {
				write(clients_list[i].sock, "m", strlen("m"));
				write(clients_list[i].sock, (char *)&id_flux, strlen((char *)&id_flux));
				write(clients_list[i].sock, data, strlen(data));
			}
		}
	}

	pthread_mutex_unlock(&mutex);
}


void *traite_connexion (void *th_client_num) {
	int client_num = *(int *)th_client_num;
	// TODO: pointer vers le client au lieu d'utiliser la liste
	//client_t client = clients_list[client_num];

	char buffer[LG_BUFFER];
	int	len_in;
	// Écanges serveur <=> client
	while ((len_in = read(clients_list[client_num].sock, buffer, LG_BUFFER)) > 0) {
		// Récupère l'action du client
		char action = (char)*buffer;

		// Récupère le numéro de flux
		int len_id_flux = read(clients_list[client_num].sock, buffer, LG_BUFFER);
		int id_flux = atoi(buffer);

		// Récupère les données du client
		char data[LG_BUFFER];
		int len_data = read(clients_list[client_num].sock, data, LG_BUFFER);

		printf(">>: %c, %d, %s\n", action, id_flux, data);

		switch (action) {
			case 's':
				// Ajoute le flux demandé à la liste d'abonnements du client
				pthread_mutex_lock(&mutex);

				clients_list[client_num].flux[clients_list[client_num].count_flux] = id_flux;
				clients_list[client_num].count_flux++;

				pthread_mutex_unlock(&mutex);
				break;
			case 'u':
				// Retire le flux demandé de la liste d'abonnements du client
				break;
			case 'p':
				// Envoie les données à tous les abonnés du flux
				send_data_to_subscribed(id_flux, data);
				break;
			case 'd':
				// Se déconnecte (proprement) du serveur
				printf("allééeeuuu\n");
				close(clients_list[client_num].sock);
				// TODO: cleaning stuff
				return NULL;
			default:
				continue;
		}

		//if (!strcmp(data, "coucou")) {
		//	send_message_to_all(data, client.sock);
		//}
		//write(STDOUT_FILENO, buffer, len_in);
		//printf("\n");
		//write(sock, buffer, len_in);
	}
	if (len_in < 0) {
		perror("read");
		exit(EXIT_FAILURE);
	}
	close(clients_list[client_num].sock);

	// TODO: traiter les clients qui quittent
	//
	// TODO: regler le broblème de double bonjour
	// NOTE: Il n'y a pas de problème
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
		clients_list[nb_clients].sock = sub_socket;

		int *th_client_num = malloc(sizeof(int *));
		*th_client_num = nb_clients;

		nb_clients++;

		// creating a thread for each client 
		pthread_create(&recvt, NULL, traite_connexion, (void *) th_client_num);

		pthread_mutex_unlock(&mutex);
	}
	return 0;
}

// Permet de gérer Ctrl+C pour quitter
int quitter_le_serveur (void) {
	return 0;
}
