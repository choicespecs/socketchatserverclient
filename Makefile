CC = gcc
CFLAGS = -g -Wall

all: chatt-server \
	chatt-client

chatt-server: chatt-server.o
	$(CC) $(CFLAGS) $@.c -lpthread -o $@

chatt-client: chatt-client.o
	$(CC) $(CFLAGS) $@.c -lpthread -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f chatt-client
	rm -f chatt-server
