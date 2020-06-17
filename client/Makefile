CC	:= gcc
CFLAGS = -g -Wall
LDFLAGS = -L. -lm

all: client

client: client.o
	$(CC) -o client $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
	rm client
