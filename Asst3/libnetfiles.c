#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>	
#include <errno.h>
#include "libnetfiles.h"

static int hostValid = -1;
char* hostName;


int socketToTheMan(char* hostname)
{
	hostName = hostname;
	int clientSocket = -1;
	int portno = 91128;
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
	struct hostent* serverIP = gethostbyname(hostname);
	if (serverIP == NULL)
	{
		hostValid = -1;
	}
	else
		hostValid = 0;
	hostName = (char*)malloc(sizeof(char)*strlen(hostname));
	strcpy(hostName, hostname);
	return hostValid; 
}


int netopen (const char* pathname, int flags)
{
	printf("PERFORMING NET OPEN\n");
	char buffer [100];
	bzero(buffer, 100);
	int rwIndicator = 0;
	int fileDes = 0;
	int successIndicator = 0;
	if (hostValid != 0)
	{
		printf ("ERROR host not found\n");
		h_errno = HOST_NOT_FOUND;
	}	
	char command [strlen(pathname)+3];
	int clientSocket = socketToTheMan(hostName);	
	sprintf(command, "%c%d%s", 'O', flags, pathname);
	//	command[0] = 'O';
//	command [1] = flags;
	rwIndicator = write(clientSocket, command, strlen(command));
	if (rwIndicator < 0)
	{
		printf("ERROR writing to socket");
	}
	rwIndicator = read(clientSocket, buffer, 100);
	sscanf(buffer, "%d,%d,%d,%d", &successIndicator, &fileDes, errno, h_errno);
	if (rwIndicator <0)
	{
		close(clientSocket);
		printf("ERROR: failed to read\n");
		return -1;
	}
	if(successIndicator == FALSE)
	{
		close(clientSocket);
		return -1;
	}
	close(clientSocket);
	return fileDes;
	}

int netclose(int fildes)
{
	char buffer [100];
	bzero(buffer, 100);
	int rwIndicator = 0;
	int clientSocket = socketToTheMan(hostName);
	if (clientSocket < 0)
	{
		printf("ERROR host not found\n");
		return -1;
		h_errno = HOST_NOT_FOUND;
	}
	sprintf(buffer, "%c,%d", 'C', fd);
	rwIndicator = write(clientSocket, buffer, strlen(buffer));
	if (rwIndicator <0)
	{
		printf("ERROR failed to write\n");
		return -1;
	}
	bzero(buffer, 100);
	rwInidcator = read(clientSocket, buffer, 100);
	if (rwIndicator < 0)
	{
		printf("ERROR failed to read\n");
		if (clientSocket !=0)
		{
			close(clientSocket);
		}
		return -1;
	}
	close(clientSocket);
	sscanf(buffer, "%d,%d,%d", &rwIndicator, &errno, &h_errno);
	return 0;
}
ssize_t netread (int fildes, void* buf, size_t nbyte)
{
	if (hostValid != 0)
	{
		printf("ERROR host not found\n");
		h_errno = HOST_NOT_FOUND;
	}
	if (fildes >=0)
	{
		printf("ERROR invalid file descriptor\n");
	}
	char* buffer = (char*)malloc(sizeof(char)*(int)nbyte+1);// [(int)nbyte+1];
	bzero(buffer, 100);
	int rwIndicator = -1;
	char command [100];
	int clientSocket = socketToTheMan(hostName);
	int numByte=0;
	sprintf(command, "%c%d;%d", 'R', fildes, (int)nbyte);
	char* readString;
	rwIndicator = write (clientSocket, command, strlen(command));
	if (rwIndicator < 0)
	{
		printf("ERROR writing to socket\n");
	}
	rwIndicator = read(clientSocket, buffer,  nbyte+1);
	if(rwIndicator < 0)
	{
		close(clientSocket);
		printf("ERROR: failed to read\n");
		return -1;
	}
	
	if (buffer[0] == FALSE)
	{
		sscanf(buffer, "%d,%d,%d,%d", &rwIndicator, &numByte, &errno, &h_errno);	
		close(clientSocket);
		return -1;
	}
	sscanf(buffer, "%d,%d,%s", &rwIndicator, &numByte, readString);
	close(clientSocket);
	printf("libnet numRead: %d\n", numByte);
	return numByte;
	}













