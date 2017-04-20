#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libnetfiles.c"
#include <fcntl.h>

int main (int argc, char** argv)
{
	int initialize = netserverinit("java.cs.rutgers.edu");
	int fileDes = netopen("merp.txt", O_RDONLY);
	printf("fildes: %d\n", fileDes);
	char buffer [100];
	bzero(buffer, 100);
	int numRead = netread((fileDes), (void*)buffer, 20); 
	printf("num read: %d\n", numRead);
}
