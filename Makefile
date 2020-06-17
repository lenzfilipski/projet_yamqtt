CC	:= gcc
CFLAGS = -g -Wall
LDFLAGS = -L. -lm

all: serveur client

serveur: serveur.o socket.c
	$(CC) -o serveur $^ $(LDFLAGS)

client: client.o socket.c
	$(CC) -o client $^ $(LDFLAGS)


pointeur: pointeur.o swap.o
	$(CC) -o pointeur $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
	rm pointeur
	rm serveur
	rm client
