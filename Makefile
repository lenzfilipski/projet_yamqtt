CC	:= gcc
CFLAGS = -g -Wall
LDFLAGS = -L. -lm

all: serveur

serveur: serveur.o socket.c
	$(CC) -o serveur $^ $(LDFLAGS)


pointeur: pointeur.o swap.o
	$(CC) -o pointeur $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
	rm pointeur
