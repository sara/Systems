#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int netopen(const char* pathname, int flags);

ssize_t netread (int fildes, void* buf, size_t nbyte);

ssize_t netwrite(int fildes, const void* buf, size_t nbyte);

int netclose (int fd);

int netserverinit(char* hostname);

int netserverinit (char* hostname, int filemode);

typedef enum {FALSE, TRUE} boolean;

