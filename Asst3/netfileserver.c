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
} clientData;


clientData* makeClient(char* command)
{
	char* pathname = (char*)malloc(sizeof(char)*strlen(command)-2);
	strcpy (pathname, command+2);
	clientData* userProfile = (clientData*)malloc(sizeof(clientData));
	userProfile -> fileMode = command[0];
	userProfile -> opMode = command[1];
	userProfile -> pathName = pathname;	
	return userProfile;
}

void destroyUser(clientData* user)
{
	free(user->pathName);
	free(user);
}

char*  myOpen(clientData* userProfile)
{
	char* buffer = (char*)malloc(sizeof(char)*100);
	bzero(buffer, 100);
	int serverFD = open(userProfile -> pathName, userProfile -> fileMode);
	if (serverFD < 0)
	{
		sprintf(buffer, "%d %d", FAIL, errno);
	}
	else
	{
		printf("SERVER FD: %d\n", serverFD);
		sprintf(buffer, "%d %d", SUCCESS, -1*serverFD);
	}
	destroyUser(userProfile);
	return buffer;
}




void* clientHandler(void* clientSocket)
{
	char* buffer;
	int clientSockFD = *(int*)clientSocket;
	char* command = (char*)malloc(sizeof(char)*1003);;
	clientData* userProfile = makeClient(command);
	read(clientSockFD, command, 1003);
	switch (userProfile -> opMode)
	{
		case 'O':
			 buffer = myOpen(userProfile);
			 break;
/*		case 'R':
			myRead (userProfile);
		case 'W':
			myWrite (userProfile);
		case 'C':
			myClose(userProfile);
*/	}
		write(clientSockFD, buffer, strlen(buffer)+1);
		pthread_exit(NULL);
		
/*	int clientSockFD = *(int*)clientSocket;
	int read_size;
	char* message;
	char clientMessage [200];
	printf("here i am!\n");
	message = "Hi, I'm Python and I'll be serving you today\n";
	write(clientSockFD, message, strlen(message));
	message = "Hi, I'm Python and I'll be serving you today\n";
	write(clientSockFD, message, strlen(message));
	read_size = recv(clientSockFD, clientMessage, 2000, 0);
	while (read_size > 0)
	{
		printf("in\n");
		clientMessage [read_size] = '\0';
		write (clientSockFD, clientMessage, strlen(clientMessage));
		memset(clientMessage, 0, 2000);
	}
	if (read_size == 0)
	{
		puts ("disconnected");
		fflush (stdout);
	}
	else if(read_size = -1)
	{
		perror("recv failed");
	}
	return 0;*/
}


int main (int argc, char** argv)
{
	boolean active = TRUE;
	int sockfd = -1;
	int clientSocket = -1;
	int portno = 9127;
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







