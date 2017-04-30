#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>

#define INVALID_FILE_MODE -99


int netopen(const char* pathname, int flags);
ssize_t netread(int fildes, void* buf, size_t nbyte);
ssize_t netwrite(int fildes, void* buf, size_t nbyte);
int netclose (int fd);
typedef enum {FALSE, TRUE} boolean;
typedef enum {UNRESTRICTED, EXCLUSIVE, TRANSACTION} mode;
int netserverinit(char* hostname, mode privacyMode);
char* hostName;
int clientSocket;
struct hostent* serverIPAddress;
struct sockaddr_in serverAddressInfo;
