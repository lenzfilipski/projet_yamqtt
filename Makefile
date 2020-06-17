CC	:= gcc
CFLAGS = -g -Wall
LDFLAGS = -L. -lm

all: serveur client

serveur: serveur.o socket.c
	$(CC) -o serveur $^ $(LDFLAGS)

client: client.o socket.c
	$(CC) -o client $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
	rm serveur
	rm client
