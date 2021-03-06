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
static mode connectionMode = -1;
static boolean connected = FALSE;

int socketToTheMan(char* hostname)
{
	hostName = hostname;
	clientSocket = -1;
	int portno = 9214;
	serverIPAddress = gethostbyname(hostName);
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
		return -1;
	}

	bzero((char*) &serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_family = AF_INET;
	bcopy((char*)serverIPAddress ->h_addr, (char*)&serverAddressInfo.sin_addr.s_addr, serverIPAddress -> h_length);
	serverAddressInfo.sin_port = htons (portno);
	if(connect(clientSocket, (struct sockaddr*)&serverAddressInfo, sizeof(serverAddressInfo)) < 0)
	{
		printf("ERROR FAILED TO CONNECT TO SERVER; errno: %d\n", errno);
		return -1;
	}
	printf("SOCKET NUMBER IS %d\n", clientSocket);
	return clientSocket;
}

int netserverinit(char* hostname, mode privacyMode)
{
	struct hostent* serverIP = gethostbyname(hostname);
	if (serverIP == NULL)
	{
		printf ("ERROR invalid hostname\n");
		errno = EINVAL;
		h_errno = HOST_NOT_FOUND;
		hostValid = -1;
	}
	else if (privacyMode <0 || privacyMode >2)
	{
		printf ("ERROR invalid connection mode\n");
		errno = EINVAL;
		h_errno = INVALID_FILE_MODE;
		hostValid = -1;
	}
	else
	{
		hostValid = 0;
		connected = TRUE;
		connectionMode = privacyMode;
		hostName = (char*)malloc(sizeof(char)*strlen(hostname)+1);
		strcpy(hostName, hostname);
	}
	return hostValid; 
}

int netopen (const char* pathname, int flags)
{
	char buffer [100];
	bzero(buffer, 100);
	int rwIndicator = 0;
	int fileDes = 0;
	int successIndicator = 0;
	if (hostValid != 0)
	{
		printf ("ERROR host not found\n");
		h_errno = HOST_NOT_FOUND;
		return -1;
	}	
	//character array the size of the path name (include a +1 for the terminating null character), then three integers to indicate privacy mode and read/write and indicate how long the string is, and a character to indicate it's an open operation
	int commandLength = strlen(pathname)+2+3*sizeof(int);
	char* command = (char*)malloc(sizeof (char)*(commandLength));
	int clientSocket = socketToTheMan(hostName);
	//indicate to server to open file with given flags and path name
	*(int*)command = commandLength;
	command[sizeof(int)] = 'O';
	*(int*)(command +sizeof(int)+1) = flags;
	*(int*)(command +2*sizeof(int)+1) = connectionMode;
	strcpy(command+(3*sizeof(int)+1), pathname);
	rwIndicator = write(clientSocket, command, commandLength);
	if (rwIndicator < 0)
	{
		printf("ERROR writing to socket");
	}
	rwIndicator = read(clientSocket, buffer, 100);
	if (rwIndicator <0)
	{
		close(clientSocket);
		printf("ERROR: failed to read\n");
		return -1;
	}
	sscanf(buffer, "%d%d%d", &successIndicator, &errno, &h_errno);
	if (successIndicator < 0)
	{
		return -1;
	}
	sscanf(buffer, "%d,%d,%d,%d", &successIndicator, &fileDes, &errno, &h_errno);
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
	int successIndicator = -1;
	int rwIndicator = 0;
	if (!connected)
	{
		printf("ERROR host_not_found\n");
		h_errno = HOST_NOT_FOUND;
		return -1;
	}
	int clientSocket = socketToTheMan(hostName);
	if (fildes >= 0)
	{	
		printf("ERROR invalid file descriptor");
		errno = EBADF;
		return -1;
	}
	if (clientSocket < 0)
	{
		printf("ERROR host not found\n");
		h_errno = HOST_NOT_FOUND;
		return -1;
	}
	int commandLength = 2*sizeof(int)+1;
	char* command = (char*)malloc(commandLength);		
	*(int *)command = commandLength;
	command[sizeof(int)] = 'C';
	*(int *)(command+sizeof(int)+1) = fildes;
	//sprintf(buffer, "%c,%d", 'C', fildes);
	rwIndicator = write(clientSocket, command, commandLength);
	if (rwIndicator <0)
	{
		printf("ERROR failed to write\n");
		return -1;
	}
	char* buffer = (char*)malloc(sizeof(char)*100);;
	bzero(buffer, 100);
	rwIndicator = read(clientSocket, buffer, 100);
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
	sscanf(buffer, "%d,%d,%d", &successIndicator, &errno, &h_errno);
	printf("CLOSE BUFFER: %s\n", buffer);
	if (successIndicator==FALSE)
	{
		return -1;
	}
	printf("SUCCESSFUL CLOSE");
	return 0;
}

ssize_t netread (int fildes, void* buf, size_t nbyte)
{
	if (hostValid != 0)
	{
		return -1;
	}
	if (fildes >=0)
	{
		printf("ERROR invalid file descriptor\n");
		errno = EBADF;
		return -1;
	}
	if (nbyte < 0)
	{
		printf("ERROR invalid read size\n");
		return -1;
	}
	//allocate enough space for the total size of the command, amount of bytes you want to read, a file descriptor, indicator that you want to read
	int commandLength = 1+3*sizeof(int);
	char* command= (char*)malloc(sizeof(char)*commandLength);// [(int)nbyte+1];
	char* buffer = (char*)malloc(sizeof(char)*(nbyte+1+sizeof(int)));
	bzero(buffer, nbyte+1+sizeof(int));
	int rwIndicator = -1;
	int numByte = -1;
	int clientSocket = socketToTheMan(hostName);
	*(int *)command = commandLength;
	command[sizeof(int)] = 'R';
	*(int *)(command + sizeof(int)+1) = fildes;
	*(int *)(command + 2*sizeof(int)+1) = nbyte;
	char readString [nbyte];
	rwIndicator = write (clientSocket, command, commandLength);
	if (rwIndicator < 0)
	{
		printf("ERROR writing to socket\n");
		close(clientSocket);
		return -1;
	}
	rwIndicator = read(clientSocket, buffer,  nbyte+1);
	if(rwIndicator < 0)
	{
		close(clientSocket);
		printf("ERROR: failed to read\n");
		return -1;
	}
	if ((int)buffer[0] < 0)
	{
		sscanf(buffer, "%d%d%d", &rwIndicator, &errno, &h_errno);
		close(clientSocket);
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
	return numByte;
	}

ssize_t netwrite (int fildes, void* buf, size_t nbyte)
{
	if (hostValid != 0)
	{
		printf("ERROR host not found\n");
		h_errno = HOST_NOT_FOUND;
		return -1;
	}
	if (fildes >=0)
	{
		printf("ERROR invalid file descriptor\n");
		errno = EBADF;
		return -1;
	}
	//length of whole command, a null byte, number of bytes to write, string to write, character to indicate write, file descriptor
	int commandLength = nbyte+2+3*sizeof(int);
	char* command = (char*)malloc(sizeof(char)*(commandLength));// [(int)nbyte+1];
	char* buffer = (char*)malloc(sizeof(char)*100);
	bzero(buffer, 100);
	int rwIndicator = -1;
	int successIndicator = -1;
	int clientSocket = socketToTheMan(hostName);	
	int num = 3*sizeof(int)+1;
	*(int *)command = commandLength;
	command[sizeof(int)] = 'W';
	*(int *)(command+sizeof(int)+1) = fildes;
	*(int *)(command+2*sizeof(int)+1) = nbyte;
	strcpy (command+num, (char*)buf);
	int numByte=0;
	rwIndicator = write (clientSocket, command, commandLength);
	if (rwIndicator < 0)
	{
		printf("ERROR writing to socket\n");
		close(clientSocket);
		return -1;
	}
	rwIndicator = read(clientSocket, buffer, sizeof(buffer));
	if(rwIndicator < 0)
	{
		close(clientSocket);
		printf("ERROR: failed to read %s\n", strerror(errno));
		return -1;
	}
	sscanf(buffer, "%d,%d,%d,%d", &successIndicator, &numByte,  &errno, &h_errno);	
	if(successIndicator == FALSE)
	{
		printf("WRITE FAILED errno: %s h_errno: %s\n", strerror(errno), strerror(h_errno));
		close(clientSocket);
		return -1;
	}
	sscanf(buffer, "%d,%d,%d", &rwIndicator, &numByte, &errno);
	close(clientSocket);
	printf("NUM WRITTEN: %d %d %d\n", rwIndicator, numByte, errno);
	return numByte;
	}
