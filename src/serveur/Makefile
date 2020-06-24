CC	:= gcc
CFLAGS = -g -Wall
LDFLAGS = -L. -lm -pthread

all: serveur

serveur: serveur.o socket.c
	$(CC) -o serveur $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
	rm serveur
