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
#define MAX_CLIENTS 50

void *traite_connexion		(void * sock);
int serveur_tcp				(const short port);
int add_client				(int sock);
void remove_client			(int client_num);
int quitter_le_serveur		(void);
int lecture_arguments		(int argc, char * argv [], short *server_port);


//
pthread_mutex_t mutex;
client_t clients_list[MAX_CLIENTS]; 
int clients_status[MAX_CLIENTS];
int pos_liste_clients = 0;


// Serveur
int main (int argc, char *argv[]) {
	short server_port = 1884;
	if (lecture_arguments(argc, argv, &server_port) < 0)
		exit(EXIT_FAILURE);
	return serveur_tcp(server_port);
}


void send_message_to_all (char *msg, int sender) {
	pthread_mutex_lock(&mutex);

	for (int i=0; i<pos_liste_clients; i++) {
		if (clients_list[i].sock != sender && clients_status[i] != 0)
			write(clients_list[i].sock, "Bonjour", strlen("Bonjour"));
	}
	
	pthread_mutex_unlock(&mutex);
}

void send_data_to_subscribed (int id_flux, char *data, int len_data) {
	printf("send: %d, %s\n", id_flux, data);
	pthread_mutex_lock(&mutex);

	for (int i=0; i<pos_liste_clients; i++) {
		for (int j=0; j<clients_list[i].count_flux; j++) {
			if (clients_list[i].flux[j] == id_flux) {
				printf("%d ", i);
				write(clients_list[i].sock, "m", LG_BUFFER);
				char char_id_flux[LG_BUFFER];
				sprintf(char_id_flux, "%d", id_flux);
				printf("%s", char_id_flux);
				write(clients_list[i].sock, char_id_flux, LG_BUFFER);
				write(clients_list[i].sock, data, LG_BUFFER);
			}
		}
	}
	printf("\n");

	pthread_mutex_unlock(&mutex);
}


void *traite_connexion (void *th_client_num) {
	int client_num = *(int *)th_client_num;
	// TODO: pointer vers le client au lieu d'utiliser la liste
	//client_t client = clients_list[client_num];

	char buffer[LG_BUFFER];
	int len_in;
	// Écanges serveur <=> client
	while ((len_in = read(clients_list[client_num].sock, buffer, LG_BUFFER)) > 0) {
		// Récupère l'action du client
		char action = (char)*buffer;

		// Récupère le numéro de flux
		
		memset(buffer,'\000',LG_BUFFER);
		/*int len_id_flux = */read(clients_list[client_num].sock, buffer, LG_BUFFER);
		int id_flux = atoi(buffer);

		// Récupère les données du client
		char data[LG_BUFFER];
		memset(data,'\000',LG_BUFFER);
		int len_data = read(clients_list[client_num].sock, data, LG_BUFFER);

		printf("%d >> %c, %d, %s\n", client_num, action, id_flux, data);

		switch (action) {
			case 's':
				// Ajoute le flux demandé à la liste d'abonnements du client
				clients_list[client_num].flux[clients_list[client_num].count_flux] = id_flux;
				clients_list[client_num].count_flux++;
				break;
			case 'u':
				// Retire le flux demandé de la liste d'abonnements du client
				for (int i=0; i<clients_list[client_num].count_flux; i++) {
					if (clients_list[client_num].flux[i] == id_flux)
						clients_list[client_num].flux[i] = 0;
				}
				break;
			case 'p':
				// Envoie les données à tous les abonnés du flux
				send_data_to_subscribed(id_flux, data, len_data);
				break;
			case 'd':
				// Se déconnecte (proprement) du serveur
				printf("allééeeuuu\n");
				remove_client(client_num);
				close(clients_list[client_num].sock);
				// TODO: cleaning stuff
				return NULL;
			default:
				continue;
		}
		memset(buffer,'\000',LG_BUFFER);
	}
	remove_client(client_num);
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

		int *th_client_num = malloc(sizeof(int *));
		// Ajoute le nouveau socket à la liste des sockets clients
		*th_client_num = add_client(sub_socket);


		// creating a thread for each client 
		pthread_create(&recvt, NULL, traite_connexion, (void *) th_client_num);

		pthread_mutex_unlock(&mutex);
	}
	return 0;
}

int add_client (int sock) {
	int i = 0;
	while (i<pos_liste_clients) {
		if (clients_status[i] == 0) {
			clients_status[i] = 1;
			clients_list[i].sock = sock;
			break;
		}
		i++;
	}
	if (i == pos_liste_clients) {
		clients_status[i] = 1;
		clients_list[i].sock = sock;
		pos_liste_clients++;
	}
	return i;
}

void remove_client (int client_num) {
	clients_status[client_num] = 0;
	for (int i=0; i<clients_list[client_num].count_flux; i++) {
		clients_list[client_num].flux[i] = 0;
	}
	printf("Removed: %d\n", client_num);
}

// Permet de gérer Ctrl+C pour quitter
int quitter_le_serveur (void) {
	return 0;
}

int lecture_arguments   (int argc, char * argv [], short *server_port) {
	char * liste_options = "p:h";
	int option;
	char * port = "1884";

	// gestion des arguments donnés au client
	while ((option = getopt(argc, argv, liste_options)) != -1) {
		switch (option) {
			case 'p' :
				port = optarg;
				break;
			case 'h' :
				fprintf(stderr, "Syntaxe : %s [-p port] \n",
						argv[0]);
				return -1;
			default :
				break;
		}
	}
	
	*server_port = (short) atoi(port);

	return 0;
}
