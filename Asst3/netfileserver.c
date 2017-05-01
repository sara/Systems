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
typedef enum {UNRESTRICTED, EXCLUSIVE, TRANSACTION} mode;
typedef struct ClientData
{
	char* pathName;
	char* writeString;
	int fileMode;
	int opMode;
	int privacyMode;
	int serverFD;
	int clientFD;
	int numBytes;
	struct ClientData* next;
} clientData;
clientData* userList;
pthread_mutex_t userListMutex;
clientData* makeClient(int clientSockFD, int commandLength)
{
	char* buffer = (char*)malloc(sizeof(char)*(commandLength - 4)); 
	bzero(buffer, commandLength-4);
	int rwIndicator = read (clientSockFD, buffer, commandLength-4);
	if (rwIndicator < 0)
	{
		printf("ERROR failed to read\n");
		close(clientSockFD);
		return NULL;
	}
	char opMode = (char)buffer[sizeof(int)-4];
	char *path;
	clientData* userProfile = (clientData*)malloc(sizeof(clientData));
	userProfile->opMode = opMode;
	userProfile -> next = NULL;
	switch (userProfile -> opMode)
	{
		case 'O':
			path = (char*)malloc(sizeof(char)*commandLength+1);
			strcpy(path, buffer+(3*sizeof(int)+1-4));
			userProfile -> fileMode = (int)buffer[sizeof(int)+1-4];
			userProfile -> pathName = path;	
			userProfile -> privacyMode = (int)buffer[2*sizeof(int)+1-4];
			break;
		case 'R':
			userProfile -> clientFD = (int)buffer[sizeof(int)+1-4];
			userProfile -> serverFD = -1*userProfile -> clientFD;
			userProfile -> numBytes = (int)buffer[2*sizeof(int)+1-4];
			break;
		case 'C':
			userProfile -> clientFD = buffer[sizeof(int)+1-4];
			userProfile -> serverFD  = -1*userProfile ->clientFD;
			break;
		case 'W':
			userProfile -> clientFD = (int)buffer[sizeof(int)+1-4];
			userProfile -> serverFD = -1*userProfile->clientFD;
			userProfile -> numBytes = (int)buffer[2*sizeof(int)+1-4];
			char* writeString = (char*)malloc(sizeof(char)*userProfile->numBytes+1);			  
			strcpy(writeString, buffer+3*sizeof(int)+1-4);
			userProfile -> writeString = writeString;
	}
	return userProfile;
}
void destroyUser(clientData* user)
{
	free(user->pathName);
	free(user);
}
int hash (char* filePath)
{
	int hash = filePath[0]%100;
	return hash;
}
boolean checkPermissions(clientData* userProfile)
{
	clientData *curr;
	pthread_mutex_lock(&userListMutex);
	for (curr = userList; curr!=NULL; curr = curr-> next)
	 {
		if (strcmp(curr->pathName, userProfile->pathName)==0)
		{
			if(curr->privacyMode == TRANSACTION)
			{
				printf("ERROR file already open in transaction mode\n");
	 			pthread_mutex_unlock(&userListMutex);
				return FALSE;
			}
			if (curr->privacyMode == EXCLUSIVE)
			{
				if (userProfile -> fileMode == O_RDONLY || curr->fileMode == O_RDONLY)
				{
	 				pthread_mutex_unlock(&userListMutex);
					return TRUE;
				}
				printf("ERROR file already open for writing in exclusive mode\n");
			
	 		pthread_mutex_unlock(&userListMutex);
			return FALSE;
			}
	 	}
	 }
	 pthread_mutex_unlock(&userListMutex);
	 return TRUE;
}
boolean isOpen(clientData* userProfile)
{
	clientData* curr = userList;
	while (curr!= NULL)
	{
		if (curr ->serverFD == userProfile->serverFD)
			return TRUE;
		curr = curr->next;
	}
	return FALSE;
}
char* myOpen(clientData* userProfile)
{
	char* buffer = (char*)malloc(sizeof(char)*100);
	bzero(buffer, 100);
	//check file table to make sure the operation is permitted with in case or preexisting files with prohibive privacy permissions
	if (checkPermissions(userProfile) == FALSE)
	{
		printf("PERMISSION DENIED\n");
		errno = EACCES;
		sprintf(buffer, "%d,%d,%d,%d", FAIL, 0, errno, h_errno);
		return buffer;
	}
	//user is allowed to proceed, set up the open
	else
	{
		int serverFD = open(userProfile -> pathName, userProfile -> fileMode);
		//check for open failures
		if (serverFD < 0)
		{
			printf("failed to open file\n");
			sprintf(buffer, "%d,%d,%d,%d", FAIL, 0, errno, h_errno);
			return buffer;
		}
		//Add the new file profile to the master hash table	
		pthread_mutex_lock(&userListMutex);
			userProfile -> next = userList;
			userList = userProfile;
		pthread_mutex_unlock(&userListMutex);
		
		sprintf(buffer, "%d,%d,%d,%d", SUCCESS, -1*serverFD, errno, h_errno);
		userProfile -> serverFD = serverFD;
		userProfile -> clientFD = -1*serverFD;
	}
	return buffer;
}
char* myRead(clientData* userProfile)
{
	char buffer[userProfile->numBytes];
	bzero(buffer, userProfile->numBytes);
	if(isOpen(userProfile) == FALSE)
	{
		printf("ERROR file descriptor does not exist\n");
		char* metaBuffer = (char*)malloc(sizeof(char)*(sizeof(int)*4+3));
		sprintf(metaBuffer,"%d,%d,%d,%d", FAIL, -1, EBADF, h_errno);
		return metaBuffer;
	}
	int numRead = read(userProfile -> serverFD, buffer, userProfile -> numBytes);
	if (numRead <0)
	{
		char* metaBuffer = (char*)malloc(sizeof(char)*(sizeof(int)*4+3));
		printf("ERROR reading from file");
		sprintf(metaBuffer, "%d,%d,%d,%d", FAIL, -1, errno, h_errno);
		return metaBuffer;
	}
	char* metaBuffer = (char*)malloc(sizeof(char)*(sizeof(int)*2+2+userProfile->numBytes));
	sprintf(metaBuffer, "%d,%d,%s", SUCCESS, numRead, buffer);
	return metaBuffer;	
	
}
char* myWrite (clientData* userProfile)
{
	char* buffer = (char*)malloc(sizeof(char)*100);
	bzero(buffer, 100);
	if(isOpen(userProfile) == FALSE)
	{
		printf("ERROR file descriptor does not exist\n");
		char* metaBuffer = (char*)malloc(sizeof(char)*(sizeof(int)*4+3));
		sprintf(metaBuffer,"%d,%d,%d,%d", FAIL, -1, EBADF, h_errno);
		return metaBuffer;
	}
	int numWritten = write(userProfile ->serverFD, userProfile->writeString, userProfile -> numBytes);	
	if (numWritten < 0)
	{
		printf("error writing  %s file descriptor = %d\n", strerror(errno), (int)userProfile ->serverFD);
		sprintf(buffer, "%d,%d,%d,%d", FAIL, -1, errno, h_errno); 
	}
	else
	{
		printf("NUM WRITTEN: %d\n", numWritten);	
		sprintf(buffer, "%d,%d,%d,%d", SUCCESS, numWritten, errno, h_errno);
	}
	return buffer;
}
char* myClose(clientData* userProfile)
{
	int serverFD = userProfile ->serverFD;
	clientData *prev, *curr;
	int closeResult;
	char* buffer = (char*)malloc(sizeof(char)*100);;
	bzero(buffer, 100);
	pthread_mutex_lock(&userListMutex);
		curr = userList;
		prev = curr;
		while (curr!= NULL && curr->serverFD!=serverFD)
		{
			prev = curr;
			curr = curr->next;
		}
		if (curr == NULL)
		{
			sprintf(buffer, "%d,%d,%d", FAIL, EBADF, h_errno);
			pthread_mutex_unlock(&userListMutex);
			printf("ERROR FILE NOT OPEN\n");
			return buffer;
		}
		if (prev == curr)
		{
			userList = userList->next;
		}
		else
		{
			prev->next = curr->next;	
		}
	pthread_mutex_unlock(&userListMutex);
	closeResult = close(serverFD);
	if (closeResult < 0)
	{
		sprintf(buffer, "%d,%d,%d", FAIL, errno, h_errno);
		return buffer;
	}
	sprintf(buffer, "%d,%d,%d", SUCCESS, errno, h_errno);
	return buffer;
}
void* clientHandler(void* clientSocket)
{
	char* buffer;
	int clientSockFD = *(int*)clientSocket;
	clientData* userProfile = NULL;
	char* commandLength = (char*)malloc(sizeof(char)*4);
	bzero(commandLength, 4);
	read(clientSockFD, commandLength, 4);
	if (read > 0)
	{
		userProfile = makeClient(clientSockFD, *(int *)commandLength);
	}
	else 
	{
		printf("ERROR READING\n");
	}
	if (userProfile == NULL)
	{
		buffer = (char*)malloc(sizeof(char)*(3*sizeof(int)));
		buffer [0] = -1;
		buffer[sizeof(int)] = errno;
		buffer[sizeof(int)] = h_errno;
		write(clientSockFD, buffer, sizeof(int));
		pthread_exit(NULL);
	}
	
	switch (userProfile -> opMode)
	{
		case 'O':
			buffer = myOpen(userProfile);
			break;
		case 'R':
			buffer = myRead(userProfile);
			break;
		case 'W':
			buffer = myWrite (userProfile);
			break;
		case 'C':
			buffer = myClose(userProfile);
			break;
	}
		write(clientSockFD, buffer, strlen(buffer)+1);
		pthread_exit(NULL);
}
int main (int argc, char** argv)
{
	boolean active = TRUE;
	int sockfd = -1;
	int clientSocket = -1;
	int portno = 9214;
	int clilen = -1;
	struct sockaddr_in serverAddressInfo, clientAddressInfo;
	pthread_mutex_init(&userListMutex, NULL);

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
		printf ("ERROR ON BINDING; errno: %d, h_errno: %d\n", errno, h_errno);
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
	return 0;
}







