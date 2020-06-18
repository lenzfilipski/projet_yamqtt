#include <stdio.h>
#include <unistd.h>

#include <stdlib.h> 
#include <string.h> 

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h> 

#include <sys/types.h>
#include <sys/socket.h>

#include "socket.h"

#define LG_BUFFER 1024

int serveur_tcp				(const short port);
int quitter_le_serveur		(void);
void traite_connexion		(int sock, int clients[], int nb_clients);


// Serveur
int main () {
	return serveur_tcp(1884);
}

void traite_connexion (int sock, int clients[], int nb_clients) {
	char				buffer[256];

	int	nb_lus;
	// Écanges serveur <=> client
	while (1) {
		if ((nb_lus = read(sock, buffer, LG_BUFFER)) == 0) {
			break;
		}
		if (nb_lus < 0) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		if (!strcmp(buffer, "coucou")) {
			for (int i=0; i<nb_clients; i++) {
				if ((clients[i] != sock)&&(clients_enabled[i]))
					write(clients[i], "Bonjour", strlen("Bonjour"));
			}
		}
		if (!strcmp(buffer, ""))
			continue;
		write(STDOUT_FILENO, buffer, nb_lus);
		write(sock, buffer, nb_lus);
		printf("\n");
	}
	close(sock);

    // TODO mettre a jour clients_enabled

}

int serveur_tcp (const short port) {
	int sock_contact;
	int sock_connectee;
	struct sockaddr_in adresse;
	socklen_t longueur;
	
	// Liste des clients
	int clients[10] ; 
    int clients_enabled[10] ;
    int nb_clients = 0;

	sock_contact = create_socket(NULL, port );
	if (sock_contact < 0)
		return -1;
	listen(sock_contact, 5);
	fprintf(stdout, "Mon adresse >> ");
	affiche_adresse_socket(sock_contact);
	while (! quitter_le_serveur()) {

		longueur = sizeof(struct sockaddr_in);
		sock_connectee = accept(sock_contact,
				(struct sockaddr *)& adresse, & longueur);
		if (sock_connectee < 0) {
			perror("accept");
			return -1;
		}
		clients_enabled[nb_clients] = 1 ;
		clients[nb_clients++] = sock_connectee;

		switch (fork()) {
			case 0 : /* fils */
				close(sock_contact);
				traite_connexion(sock_connectee, clients, nb_clients);
				exit(EXIT_SUCCESS);
			case -1:
				perror("fork");
				return -1;
			default : /* père */
				close(sock_connectee);
				nb_clients--;
		}
	}
	return 0;
}

// Permet de gérer Ctrl+C pour quitter
int quitter_le_serveur (void) {
	return 0;
}
