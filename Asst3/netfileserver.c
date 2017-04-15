#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


void error(char* msg)
{
	perror(msg);
	exit (1);
}

void* clientHandler(void* clientSocket)
{
	int clientSockFD = *(int*)clientSocket;
	int commandSize = 
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
	int sockfd = -1;
	int clientSocket = -1;
	int portno = 8080;
	int clilen = -1;
	int amtData = -1;
	char buffer [256];
	struct sockaddr_in serverAddressInfo;
	struct sockaddr_in clientAddressInfo;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		error("ERROR opening socket");
	}
	bzero((char *)&serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_port = htons(portno);
	serverAddressInfo.sin_family = AF_INET;	
	serverAddressInfo.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(sockfd, (struct sockaddr *) &serverAddressInfo, sizeof(serverAddressInfo))<0)
	{
		error ("ERROR ON BINDING");
	}
		
		listen (sockfd, 5);
		clilen  = sizeof(clientAddressInfo);
		pthread_t clientThread;
		
	while (clientSocket = accept(sockfd, (struct sockaddr *) &clientAddressInfo, &clilen))
	{
		if (clientSocket <0)
		{
			error("ERROR on accept");
		}
		else if (pthread_create(&clientThread, NULL, clientHandler, (void*)&clientSocket)<0)
		{
			error("ERROR could not create thread");
		}
		pthread_join(clientThread, NULL); 

	}
}







