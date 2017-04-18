#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>

typedef enum {FAIL, SUCCESS} status;
typedef enum {FALSE, TRUE} boolean;
typedef struct ClientData
{
	char* pathName;
	int fileMode;
	int opMode;
	int privacyMode;
	int serverFD;
	int clientFD;
	int numBytes;
	struct ClientData* next;
} clientData;
typedef struct dataTable
{
	clientData* files [100];
}dataTable;
dataTable* fileTable;
clientData* makeClient(char* command)
{
	printf("command: %s\n", command);
	char buffer [1000];
	bzero(buffer, 1000);
	int fileMode, fileDes, nbyte;
	char* path;
	char userMode;
	clientData* userProfile = (clientData*)malloc(sizeof(clientData));
	userProfile -> opMode = command[0];
	userProfile -> next = NULL;
	switch (userProfile -> opMode)
	{
		case 'O':
			//sscanf(command+1, "%d", fileMode);
			path = (char*)malloc(sizeof(char)*strlen(command)-2);
			sscanf(command, "%c%d%s", &userMode, &fileMode, path);
			//strcpy(path, command+2);
			printf("file mode: %d, path: %s\n", fileMode, path);
			userProfile -> fileMode = fileMode;
			userProfile -> pathName = path;	
			break;
		case 'R':
			sscanf(command+1, "%d%d", fileDes, nbyte);
			userProfile -> serverFD = fileDes*-1;
			userProfile -> clientFD = fileDes;
			userProfile -> numBytes = nbyte;
			printf("FILE DES: %d\n", userProfile-> numBytes);
			printf("NUM BYTES: %d\n", userProfile->numBytes);
			break;

	}
	return userProfile;
}
void makeTable()
{
	int i;
	fileTable = (dataTable*)malloc(sizeof(dataTable));
	for (i=0; i< 100; i++)
	{
		fileTable->files[i] = NULL;
	}
}
void destroyUser(clientData* user)
{
	free(user->pathName);
	free(user);
}
int hash (int fileDes)
{
	int hash = fileDes%13;
	return hash;
}
boolean isOpen(int fileDes)
{
	clientData* curr = fileTable->files[hash(fileDes)];
	while (curr!= NULL)
	{
		if (curr ->serverFD == fileDes)
				return TRUE;
	}
	return FALSE;
}
char* myOpen(clientData* userProfile)
{
	char* buffer = (char*)malloc(sizeof(char)*100);
	bzero(buffer, 100);
//	printf("%s\n", userProfile->pathName);
	int serverFD = open(userProfile -> pathName, userProfile -> fileMode);
	if (serverFD < 0)
	{
		printf("failed to open file\n");
		sprintf(buffer, "%d %d", FAIL, errno);
	}
	else
	{
		int hashIndex = hash(userProfile->serverFD);
	//	printf("SERVER FD: %d\n", serverFD);
		sprintf(buffer, "%d %d", SUCCESS, -1*serverFD);
		userProfile -> serverFD = serverFD;
		userProfile -> clientFD = -1*serverFD;
		userProfile -> next = fileTable -> files [hashIndex];
	}
	return buffer;
}
char* myRead(clientData* userProfile)
{
	char* stringNum;
	sprintf(stringNum, "%d", userProfile ->numBytes);
	int bufferLength = userProfile->numBytes +strlen(stringNum) +1;
	char buffer[userProfile->numBytes];
	char* metaBuffer = (char*)malloc(sizeof(char)* bufferLength);
	bzero(metaBuffer, bufferLength);
	printf("118\n");
	if(isOpen(userProfile -> serverFD) == FALSE)
	{
		printf("ERROR file descriptor does not exist\n");
		sprintf(metaBuffer,"%d%s", FAIL, strerror(9));
	}
	printf("126\n");
	//in extra credit check that file is allowed to be read from
	int numRead = read(userProfile -> serverFD, buffer, userProfile -> numBytes);
	if (numRead <0)
	{
		printf("ERROR reading from file");
		sprintf(metaBuffer, "%d%d", FAIL, errno);
		return metaBuffer;
	}
	sprintf(metaBuffer, "%d;%s", numRead+1, buffer);
	return metaBuffer;	
	
}
void* clientHandler(void* clientSocket)
{
	char* buffer;
	int clientSockFD = *(int*)clientSocket;
	char* command = (char*)malloc(sizeof(char)*1003);
	read(clientSockFD, command, 1003);
	clientData* userProfile = makeClient(command);
	switch (userProfile -> opMode)
	{
		case 'O':
			 buffer = myOpen(userProfile);
			 break;
		case 'R':
			buffer = myRead (userProfile);
			break;
/*		case 'W':
			myWrite (userProfile);
		case 'C':
			myClose(userProfile);
*/	}
		write(clientSockFD, buffer, strlen(buffer)+1);
		pthread_exit(NULL);
}
int main (int argc, char** argv)
{
	boolean active = TRUE;
	int sockfd = -1;
	int clientSocket = -1;
	int portno = 91128;
	int clilen = -1;
	int amtData = -1;
	char buffer [5000];
	struct sockaddr_in serverAddressInfo;
	struct sockaddr_in clientAddressInfo;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("ERROR opening socket; errno: %d, h_errno:%d\n", errno, h_errno);
		return 1;
	}
	bzero((char *)&serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_port = htons(portno);
	serverAddressInfo.sin_family = AF_INET;	
	serverAddressInfo.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr *) &serverAddressInfo, sizeof(serverAddressInfo))<0)
	{
		printf ("ERROR ON BINDING; errno: %d, h_errno: ", errno, h_errno);
		return 1;
	}
		if (listen (sockfd, 50)<0)
		{
			printf("ERROR LISTEN FAILED; errno: %d", errno);
			close(sockfd);
			return 1;
		}
	clilen  = sizeof(clientAddressInfo);
	pthread_t clientThread;
	makeTable(fileTable);
	while (active == TRUE)
	{
		printf("Listening for client requests\n");
		clientSocket = accept(sockfd, (struct sockaddr *) &clientAddressInfo, &clilen);
		if (clientSocket <0)
		{
			if (errno == EINTR)
			{
				printf("process completed succesfully\n");
				close(clientSocket);
				close(sockfd);
				return 0;
			}
			else
			{
				printf("ERROR on accept, errno: %d\n", errno);
				close (clientSocket);
				return 1;
			}
		}
		else if (pthread_create(&clientThread, NULL, clientHandler, (void*)&clientSocket)!=0)
		{
			printf("ERROR could not create thread; errno: %d\n", errno);
		}
		pthread_join(clientThread, NULL); 
	
	}
}







