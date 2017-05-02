CC gcc
CCFLAGS = -Wall

all: libnetfiles netfileserver

libnetfiles: libnetfiles.h libnetfiles.c
	$(CC) $(CCFLAGS)  libnetfiles.c netclient.c -o netclient

netfileserver: libnetfiles.h netfileserver.c
	$(CC) $(CCFLAGS) -pthread -o netfileserver netfileserver.c



clean:
	rm client server
