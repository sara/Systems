#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>	
#include <errno.h>
#include <h_errno.h>
#include "libnetfiles.h"
static boolean hostValid = FALSE;
static char* hostName;


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
	char buffer [100];
	int writeIndicator = 0;
	if (hostValid != 1)
	{
		printf ("ERROR host not found\n");
		h_errno = HOST_NOT_FOUND;
	}
	
	char command [strlen(pathname)+3];
	int clientSocket = socketToTheMan(hostName);	
	strcpy(command+2, pathname);
	command[0] = 'O';
	command [1] = flags;
	writeIndicator = write(clientSocket, command, strlen(command));
	if (writeIndicator < 0)
	{
		error("ERROR writing to socket");
	}
	int readIndicator = read(clientSocket, buffer, 100);
	if (readIndicator <0)
	{
		close(clientSocket);
		return -1;
	}
	if (buffer[0] == FALSE)
	{
		errno =  buffer[1];
		close(clientSocket);
		return -1;
	}
	close(clientSocket);
	return buffer[1];
}















