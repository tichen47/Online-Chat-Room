CC = gcc
CFLAGS = -Wall -g

all: client server
client: client.o
	$(CC) -pthread -o client client.o
server: server.o
	$(CC) -pthread -o server server.o

client.o: client.c client.h
	$(CC) $(CFLAGS) -c client.c
server.o: server.c server.h client.h
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f client client.o server server.o