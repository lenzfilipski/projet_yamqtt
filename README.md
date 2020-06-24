# projet yamqtt

Ce projet consiste à réaliser un client et un serveur en C fonctionnant de manière similaire à mqtt.

# Consignes
Les consignes sont accéssibles dans [CONSIGNES.md](./CONSIGNES.md).

# Protocole
Le protocole utilisé est décrit dans [yamqtt\_protocole.pdf](./yamqtt_protocole.pdf).

# Utilisation du client
## Mode ligne de commande
```
Utilisation
	client [-a adresse] [-p port] [-f flux [fichier]]
	client [-a adresse] [-p port] [-s flux]

Options
-a
	l'adresse du serveur yamqtt
-p
	le port du serveur yamqtt
-f
	le flux sur lequel publier. Cette option ne permet au client de publier que sur un seul flux. 
	Cette option ne permet pas au client de s'abonner à un ou des flux.
-s
	le flux auquel s'abonner. Cette option ne permet au client de s'abonner qu'à un seul flux. 
	Cette option ne permet pas au client de publier.

Si -f ou -s ne sont pas utilisés, le client s'exécute en mode interractif.
```
