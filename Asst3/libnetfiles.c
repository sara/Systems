#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>	
#include <errno.h>
#include "libnetfiles.h"

static boolean hostValid = FALSE;
static char* hostName;



void error (char* msg)
{
	perror(msg);
	exit(0);
}

int socketToTheMan(char* hostname)
{
	hostName = hostname;
	int clientSocket =- 1;
	int portno = 8080;
	struct sockaddr_in serverAddressInfo;
	struct hostent* serverIPAddress = gethostbyname(hostName);
	if (serverIPAddress = NULL)
	{
		error("ERROR no such host");
	}
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket < 0)
	{
		error ("ERROR creating socket");
	}

	bzero((char*) &serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_family = AF_INET;
	serverAddressInfo.sin_port = htons (portno);
	bcopy((char*)serverIPAddress ->h_addr, (char*)&serverAddressInfo.sin_addr.s_addr, serverIPAddress -> h_length);

	if(connect(clientSocket, (struct sockaddr*)&serverAddressInfo, sizeof(serverAddressInfo)) < 0)
	{
		error("ERROR connecting");
	}
	return clientSocket;
}



	









int netserverinit(char* hostname)
{
	struct sockaddr_in sa;
	int connection = inet_pton(AF_INET, hostname, &(sa.sin_addr));
	if (connection < 0)
	hostValid = FALSE;
	else	
	hostValid = TRUE;
}


int netopen (const char* pathname, int flags)
{
	int writeIndicator = 0;
	if (hostValid != 1)
	{
		error ("ERROR host not found");
	}
	
	char command [sizeof(pathname) +1];
	int clientSocket = socketToTheMan(hostName);	
	strcpy(command, pathname);
	command[sizeof(command)-1] = flags;
	writeIndicator = write(clientSocket, command, strlen(command));
	if (writeIndicator < 0)
	{
		error("ERROR writing to socket");
	}


}















