#include <stdio.h>
#include <unistd.h>

#include <stdlib.h> 
#include <string.h> 

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h> 

#include <sys/types.h>
#include <sys/socket.h>

int create_socket (const char* ip_addr, const short port) {

	int sock;
	struct sockaddr_in adresse;

	// création d'un socket: int socket (int domaine, int type, int protocole);
	// Le mode TCP est automatiquement choisi avec SOCK_STREAM
	// mode connecté ---------------.
	//          IPv4 ----.          |
	//                   |          |
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		// print une description du code d'erreur
		perror("socket");
		return -1;
	}

	memset(& adresse, 0, sizeof (struct sockaddr_in));
	adresse.sin_family = AF_INET;
	// htons() retourne la valeur en big-endian ce qui est indispensable pour e réseau
	adresse.sin_port = htons(port);
	// htons pour short et htonl pour long ?
	adresse.sin_addr.s_addr = htonl(INADDR_ANY);

	// Associe un objet addresse à un socket
	// taille de l'objet adresse ------------------------------.
	// objet adresse -----------.                              |
	// socket -.                |                              |
	//         |   |<___________^_____________>|  |<___________^__________>|
	if (bind(sock, (struct sockaddr *) & adresse, sizeof(struct sockaddr_in)) < 0) {
		close(sock);
		perror("bind");
		return -1;
	}
	return sock;
}

int affiche_adresse_socket (int sock) {
	struct sockaddr_in adresse;
	socklen_t longueur ;
	longueur = sizeof(struct sockaddr_in);
	if (getsockname(sock, (struct sockaddr *) & adresse, & longueur) < 0) {
		perror("getsockname");
		return -1;
	}
	fprintf(stdout, "IP = %s, Port = %u \n",
			inet_ntoa(adresse.sin_addr),
			ntohs(adresse.sin_port));
	return 0;
}

