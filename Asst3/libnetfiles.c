#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>	
#include <errno.h>
#include "libnetfiles.h"

static boolean hostValid = FALSE;
static char* hostName;


int socketToTheMan(char* hostname)
{
	hostName = hostname;
	int clientSocket = -1;
	int portno = 9127;
	struct sockaddr_in serverAddressInfo;
	struct hostent* serverIPAddress = gethostbyname(hostName);
	if (serverIPAddress == NULL)
	{
		printf("ERROR INVALID HOST NAME\n");
		h_errno = HOST_NOT_FOUND;
		return -1;
	}
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket < 0)
	{
		printf("ERROR FAILED TO CREATE CLIENT SOCKET\n");
		return -2;
	}

	bzero((char*) &serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_family = AF_INET;
	bcopy((char*)serverIPAddress ->h_addr, (char*)&serverAddressInfo.sin_addr.s_addr, serverIPAddress -> h_length);
	serverAddressInfo.sin_port = htons (portno);
	if(connect(clientSocket, (struct sockaddr*)&serverAddressInfo, sizeof(serverAddressInfo)) < 0)
	{
		printf("ERROR FAILED TO CONNECT TO SERVER; errno: %d\n", errno);
		return -3;
	}
	return clientSocket;
}




int netserverinit(char* hostname)
{
	/*int hostValid;
	struct hostent* serverIP = gethostbyname(hostname);
	if (serverIP == NULL)
	{
		hostValid = -1;
	}
	host valid = 0;
	//int hostValid = socketToTheMan(hostname);
	if (serverIPAddress == NULL)
	{
			hostValid = 0;
			printf("yeet\n");
	}
	else
	{
		hostValid = 1;
		printf("yoot\n");
	}*/
	int num = socketToTheMan(hostname);
	return num;
}


int netopen (const char* pathname, int flags)
{
	char buffer [100];
	bzero(buffer, 100);
	int writeIndicator = 0;
	int fileDes = 0;
	if (hostValid != 1)
	{
		printf ("ERROR host not found\n");
		h_errno = HOST_NOT_FOUND;
	}	
	char command [strlen(pathname)+3];
	int clientSocket = socketToTheMan(hostName);	
	strcpy(command+2, pathname);
	sprintf(command, "%c%d", 'O', flags);
//	command[0] = 'O';
//	command [1] = flags;
	writeIndicator = write(clientSocket, command, strlen(command));
	if (writeIndicator < 0)
	{
		error("ERROR writing to socket");
	}
	int readIndicator = read(clientSocket, buffer, 100);
	sscanf(buffer, "%d%d", &readIndicator, &fileDes);
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
	return fileDes;
}















