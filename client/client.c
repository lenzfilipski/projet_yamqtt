#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

// threads pour écouter et envoyer en parallèle
#include <pthread.h>

#define LG_BUFFER 1024

int lecture_arguments	(int argc, char * argv [],
						struct sockaddr_in * adresse,
						char * protocole);
void *handle_in_msg		(void *t_sock);

// Client
int main (int argc, char *argv[]) {

	int		sock;
	struct	sockaddr_in adresse;
	char	buffer[LG_BUFFER];
	
	pthread_t listen;

	if (lecture_arguments(argc, argv, & adresse, "tcp") < 0)
		exit(EXIT_FAILURE);
	adresse.sin_family = AF_INET;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	if (connect(sock, (struct sockaddr *) & adresse,
				sizeof (struct sockaddr_in)) < 0) {
		perror("connect");
		exit(EXIT_FAILURE);
	}
	setvbuf(stdout, NULL, _IONBF, 0);

	int *t_sock = malloc(sizeof(int *));
	*t_sock = sock;
	pthread_create(&listen, NULL, handle_in_msg, (void *) t_sock);

	while (1) {
		scanf("%s", buffer);
		write(sock, buffer, strlen(buffer));
	}

	return EXIT_SUCCESS;
	//return 0;
}

void *handle_in_msg (void *t_sock) {
	int sock = *(int *) t_sock;
	char buffer[LG_BUFFER];
	int len_in;
	while ((len_in = read(sock, buffer, LG_BUFFER)) > 0) {
		write(STDOUT_FILENO, buffer, len_in);
		printf("\n");
	}

	return NULL;
}

int lecture_arguments 	(int argc, char * argv [],
						struct sockaddr_in * adresse,
						char * protocole) {
	char * liste_options = "a:p:h";
	int option;
	char * hote = "localhost";
	char * port = "1884";
	struct hostent * hostent;
	struct servent * servent;
	int numero;

	// gestion des arguments donnés au client
	while ((option = getopt(argc, argv, liste_options)) != -1) {
		switch (option) {
			case 'a' :
				hote = optarg;
				break;
			case 'p' :
				port = optarg;
				break;
			case 'h' :
				fprintf(stderr, "Syntaxe : %s [-a adresse] [-p port] \n",
						argv[0]);
				return -1;
			default :
				break;
		}
	}
	memset(adresse, 0, sizeof (struct sockaddr_in));
		if (inet_aton(hote, & (adresse->sin_addr)) == 0) {
			if ((hostent = gethostbyname(hote)) == NULL) {
				fprintf(stderr, "hôte %s inconnu \n", hote);
				return -1;
			}
			adresse->sin_addr.s_addr =
				((struct in_addr *) (hostent->h_addr))->s_addr;
		}
	if (sscanf(port, "%d", & numero) == 1) {
		adresse->sin_port = htons(numero);
		return 0;
	}
	if ((servent = getservbyname(port, protocole)) == NULL) {
		fprintf(stderr, "Service %s inconnu \n", port);
		return -1;
	}
	adresse->sin_port = servent->s_port;
	return 0;
}
