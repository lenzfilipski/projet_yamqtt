# projet yamqtt

Ce projet consiste à réaliser un client et un serveur en C fonctionnant de manière similaire à mqtt.

# Consignes
Les consignes sont accéssibles dans [CONSIGNES.md](./CONSIGNES.md).

# Protocole
Le protocole utilisé est décrit dans [yamqtt\_protocole.pdf](./yamqtt_protocole.pdf).

# Construire et installer le projet
- cd build
- cmake ..
- make
- sudo make install

Deux exécutables sont créés:
- `serveur_yamqtt`
- `client_yamqtt`

# Utilisation du client
```
Utilisation
	client_yamqtt [-a adresse] [-p port]
	client_yamqtt [-a adresse] [-p port] -f flux
	client_yamqtt [-a adresse] [-p port] -s flux

Options
-a
	l'adresse du serveur yamqtt. Utilise l'adresse 'localhost' par défaut.
-p
	le port du serveur yamqtt. Utilise le port 1884 par défaut.
-f
	le flux sur lequel publier. Cette option ne permet au client de publier que sur un seul flux. 
	Cette option ne permet pas au client de s'abonner à un ou des flux.
-s
	le flux auquel s'abonner. Cette option ne permet au client de s'abonner qu'à un seul flux. 
	Cette option ne permet pas au client de publier.

On peut utiliser les pipes stdin/stdout avec les options -f et -s.
Si -f ou -s ne sont pas utilisés, le client s'exécute en mode interractif.
```

# Utilisation du serveur
```
Utilisation
	serveur_yamqtt [-p port]

Options
-p
	le port sur lequel écouter. Écoute sur le port 1884 par défaut.
```
