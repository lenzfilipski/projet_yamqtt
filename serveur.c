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
void traite_connexion		(int sock);


// Serveur
int main () {
	return serveur_tcp(1884);
}

void traite_connexion (int sock) {
	struct sockaddr_in	adresse;
	socklen_t			longueur;
	char				buffer[256];

	longueur = sizeof(struct sockaddr_in);
	if (getpeername(sock,(struct sockaddr *)& adresse,& longueur) < 0) {
		perror("getpeername");
		return;
	}

	/*
	sprintf(buffer, "IP = %s, Port = %u \n",
			inet_ntoa(adresse.sin_addr),
			ntohs(adresse.sin_port));
	fprintf(stdout, "Connexion : locale ");
	affiche_adresse_socket(sock);
	fprintf(stdout, " distante %s", buffer);
	write(sock, "Votre adresse : ", 16);
	write(sock, buffer, strlen(buffer));
	*/
	int	nb_lus;
	nb_lus = read(sock, buffer, LG_BUFFER);
	write(STDOUT_FILENO, buffer, nb_lus);
	sprintf(buffer, "Bonjour");
	write(sock, buffer, strlen(buffer));

	// Écanges serveur <=> client
	/*
	int nb_lus;
	while (1) {	
		if ((nb_lus = read(sock, buffer, LG_BUFFER)) == 0) {
			break;
		}
		if (nb_lus < 0) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		write(STDOUT_FILENO, buffer, nb_lus);
	}
	*/
	close(sock);
}

int serveur_tcp (const short port) {
	int sock_contact;
	int sock_connectee;
	struct sockaddr_in adresse;
	socklen_t longueur;

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
		switch (fork()) {
			case 0 : /* fils */
				close(sock_contact);
				traite_connexion(sock_connectee);
				exit(EXIT_SUCCESS);
			case -1:
				perror("fork");
				return -1;
			default : /* père */
				close(sock_connectee);
		}
	}
	return 0;
}

// Permet de gérer Ctrl+C pour quitter
int quitter_le_serveur (void) {
	return 0;
}
