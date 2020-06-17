#ifndef SOCKET_H
#define SOCKET_H

int create_socket			(const char *, const short);

int affiche_adresse_socket	(int sock);
int serveur_tcp				(const short port);
int quitter_le_serveur		(void);
void traite_connexion		(int sock);

#endif
