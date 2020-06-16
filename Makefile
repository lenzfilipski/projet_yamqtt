CC	:= gcc
CFLAGS = -g -Wall
LDFLAGS = -L. -lm

all: pointeur

pointeur: pointeur.o swap.o
	$(CC) -o pointeur $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
	rm pointeur
