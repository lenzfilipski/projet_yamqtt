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

int mode_client = 0;
int flux_client;

int lecture_arguments	(int argc, char * argv [],
						struct sockaddr_in * adresse,
						char * protocole);
void *handle_in_msg		(void *t_sock);
void print_message		(int id_flux, char *data);

// Client
int main (int argc, char *argv[]) {

	for (int i=0; i<argc; i++) {
		printf("%s, ", argv[i]);
	}
	printf("\n");

	int		sock;
	struct	sockaddr_in adresse;
	
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

	switch (mode_client) {
		case 0:
			// Mode interractif
			{
			int *t_sock = malloc(sizeof(int *));
			*t_sock = sock;
			pthread_create(&listen, NULL, handle_in_msg, (void *) t_sock);
			while (1) {
				char buffer[LG_BUFFER];
				fgets(buffer, LG_BUFFER, stdin);
				// supprime le caractère de fin de ligne
				buffer[strlen(buffer)-1] = '\000';
				write(sock, buffer, LG_BUFFER);
			}
			return EXIT_SUCCESS;
			}
			break;
		case 1:
			//Mode poblicateur
			{
			char buffer[LG_BUFFER];
			memset(buffer, '\000', LG_BUFFER);
			int end_of_file = 0;
			while (!end_of_file) {
				fgets(buffer, LG_BUFFER, stdin);
				// regarde si la fin du pipe est atteinte
				if (buffer[strlen(buffer)-1] == '\000')
					end_of_file = 1;
				// supprime le caractère de fin de ligne
				buffer[strlen(buffer)-1] = '\000';
				//printf("%d %s\n", len_in, buffer);
				write(sock, "p", LG_BUFFER);
				char char_id_flux[LG_BUFFER];
				sprintf(char_id_flux, "%d", flux_client);
				write(sock, char_id_flux, LG_BUFFER);
				write(sock, buffer, LG_BUFFER);
				//write(1, buffer, LG_BUFFER);
				memset(buffer, '\000', LG_BUFFER);
			}
			}
			break;
		case 2:
			// Mode abonné
			{

			}
			break;
		default:
			break;
	}
}

void *handle_in_msg (void *t_sock) {
	int sock = *(int *) t_sock;
	char buf_action[10];
	int len_in;
	while ((len_in = read(sock, buf_action, LG_BUFFER)) > 0) {
		// Récupère l'action du client
		char action = *buf_action;
		//printf("in: %c, ", action);

		// Récupère le numéro de flux
		char buf_id_flux[50];
		memset(buf_id_flux, '\000', 50);
		/*int len_id_flux = */read(sock, buf_id_flux, LG_BUFFER);
		int id_flux = atoi(buf_id_flux);
		//printf("%d, ", id_flux);

		// Récupère les données du client
		char data[LG_BUFFER];
		memset(data, '\000', LG_BUFFER);
		/*int len_data = */read(sock, data, LG_BUFFER);
		//printf("%s\n", data);

		switch (action) {
			case 'm':
				// Gère la réception d'un message
				print_message(id_flux, data);
				break;
			case 'e':
				// TODO: Handle error message
				break;
			default:
				printf("NOTHING...\n");
				continue;
		}

		//write(STDOUT_FILENO, buffer, len_in);
		//printf("\n");
		memset(buf_action,'\000', 10);
	}

	return NULL;
}

void print_message (int id_flux, char *data) {
	printf("[%d] >> %s\n", id_flux, data);
}

int lecture_arguments 	(int argc, char * argv [],
						struct sockaddr_in * adresse,
						char * protocole) {
	char * liste_options = "a:p:f:s:h";
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
			case 'f':
				if (mode_client == 0) {
					mode_client = 1;
					flux_client = atoi(optarg);
				}
				break;
			case 's':
				if (mode_client == 0) {
					mode_client = 2;
					flux_client = atoi(optarg);
				}
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
