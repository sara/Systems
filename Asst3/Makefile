CC = gcc
CCFLAGS = -Wall

all: libnetfiles netfileserver

libnetfiles: libnetfiles.h libnetfiles.c
	$(CC) $(CCFLAGS) -o  client libnetfiles.c

netfileserver: libnetfiles.h netfileserver.c
	$(CC) $(CCFLAGS) -pthread -o server netfileserver.c



clean:
	rm - rf client server
