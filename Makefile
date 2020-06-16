CC	:= gcc
CFLAGS = -g -Wall

main: pointeur.c
	$(CC) $(CFLAGS) pointeur.c -o pointeur

