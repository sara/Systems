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


//NEED TO ADD IN CLIENT ID TO HASH TABLE!!!!
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
typedef struct dataTable
{
	clientData* files [100];
}dataTable;
dataTable* fileTable;
pthread_mutex_t fileTableMutex;
clientData* makeClient(char* command)
{
	//printf("command: %s\n", command);
	char buffer [1000];
	bzero(buffer, 1000);
	int fileMode, fileDes, nbyte;
	char *path, *writeString, *garbageString;
	char privacyMode, userMode;
	clientData* userProfile = (clientData*)malloc(sizeof(clientData));
	userProfile -> opMode = command[0];
	userProfile -> next = NULL;
	switch (userProfile -> opMode)
	{
		case 'O':
			//sscanf(command+1, "%d", fileMode);
			path = (char*)malloc(sizeof(char)*strlen(command)-2);
			sscanf(command, "%c,%d,%d,%s", &userMode, &fileMode, &privacyMode, path);
			//strcpy(path, command+2);
			//printf("file mode: %d, path: %s\n", fileMode, path);
			userProfile -> fileMode = fileMode;
			userProfile -> pathName = path;	
			userProfile -> privacyMode = privacyMode;
			break;
		case 'R':
			sscanf(command+1, "%d;%d", &fileDes, &nbyte);
			userProfile -> serverFD = fileDes*-1;
			userProfile -> clientFD = fileDes;
			userProfile -> numBytes = nbyte;
			//printf("fildes: %d nbyte: %d\n", fileDes, nbyte);
			//printf("FILE DES: %d\n", userProfile-> numBytes);
			//printf("NUM BYTES: %d\n", userProfile->numBytes);
			break;
		case 'C':
			userProfile->serverFD = sscanf(command+1, "%d", &fileDes);
			break;
		case 'W':
			writeString = (char*)malloc(sizeof(char)*strlen(command));
			sscanf(command+2, "%d,%d", &fileDes, &nbyte);
			garbageString = (char*)malloc(sizeof(char) * (nbyte+(fileDes*-1)+4));
			sprintf(garbageString, "%c,%d,%d", 'W', fileDes, nbyte);
			strncpy(writeString, command+strlen(garbageString), strlen(writeString)-strlen(garbageString));
			free(garbageString);
			userProfile -> clientFD = fileDes;
			userProfile -> serverFD = -1*fileDes;
			userProfile -> writeString = writeString;
			userProfile -> numBytes = nbyte; 
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
	int hash = fileDes%100;
	return hash;
}
boolean checkPermissions(clientData* userProfile)
{
	 clientData *curr, *prev;
	 for (curr = fileTable->files[hash(userProfile->serverFD)]; curr!=NULL; curr = curr-> next)
	 {
		if (curr->serverFD == userProfile->serverFD)
		{
			if(curr->privacyMode == TRANSACTION)
			{
				return FALSE;
			}
			if (curr->privacyMode == EXCLUSIVE)
			{
				if (userProfile -> opMode == 'R'
				{
					return TRUE;
				}
				if(curr->opMode == 'R'
				{
					return TRUE;
				}
			}
			return FALSE;
		}
	 }
	 return TRUE;
}
boolean isOpen(clientData* userProfile)
{
	int hashIndex = hash(userProfile->serverFD);
	//printf("isOpen fileDes: %d isOpen hash index: %d\n", userProfile->serverFD, hashIndex);
	//printf("READ STATUS: %d\n", fileTable->files[hashIndex]->serverFD);
	
	clientData* curr = fileTable->files[hash(userProfile->serverFD)];
	while (curr!= NULL)
	{
		//printf("TABLE STATE: %d\n", fileTable ->files[hash(userProfile->serverFD)]->serverFD);
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
//	printf("%s\n", userProfile->pathName);
	int serverFD = open(userProfile -> pathName, userProfile -> fileMode);
	if (serverFD < 0)
	{
		printf("failed to open file\n");
		sprintf(buffer, "%d,%d,%d,%d", FAIL, 0, errno, h_errno);
	}
	
	//check file table to make sure the operation is permitted with in case or preexisting files with 
	//prohibive privacy permissions
	
	if (!checkPermissions)
	{
		printf("PERMISSION DENIED\n");
		errno = EACCES;
		sprintf(buffer, "%d,%d,%d,%d", FAIL, 0, errno, h_errno);
		return buffer;
	}
	
	//user is allowed to proceed, set up the open
	else
	{
		int hashIndex = (serverFD)%100;
		//printf("myOpen file descriptor: %d, hashIndex %d\n");
		userProfile -> next = fileTable->files[hashIndex];
		fileTable->files[hashIndex] = userProfile;
		sprintf(buffer, "%d,%d,%d,%d", SUCCESS, -1*serverFD, errno, h_errno);
		userProfile -> serverFD = serverFD;
		userProfile -> clientFD = -1*serverFD;
	//	printf("OPEN FILE DES: %d OPEN HASH INDEX: %d\n", userProfile->serverFD, hashIndex);	
				
		//printf("OPEN TABLE STATE: %d\n", fileTable ->files[hash(hashIndex)]->serverFD);
	}
	
	return buffer;
}
char* myRead(clientData* userProfile)
{
	char* stringNum = (char*)malloc(sizeof(char)*userProfile->numBytes+1);
	sprintf(stringNum, "%d", userProfile ->numBytes);
	int bufferLength = userProfile->numBytes +strlen(stringNum) +1;
	char buffer[userProfile->numBytes];
	bzero(buffer, userProfile->numBytes);
	char* metaBuffer = (char*)malloc(sizeof(char)* bufferLength);
	bzero(metaBuffer, bufferLength);
	if(isOpen(userProfile) == FALSE)
	{
		printf("ERROR file descriptor does not exist\n");
		sprintf(metaBuffer,"%d,%d,%d,%d", FAIL, -1, EBADF, h_errno);
		return metaBuffer;
	}

	if (!checkPermissions)
	{
		printf("ERROR pemission denied\n");
		sprintf(metaBuffer, "%d,%d,%d,%d", FAIL, -1, EACCES, h_errno);
		return metaBuffer;
	}
	
	
	pthread_mutex_lock(&fileTableMutex);
	int numRead = read(userProfile -> serverFD, buffer, userProfile -> numBytes);
	pthread_mutex_lock(&fileTableMutex);
	if (numRead <0)
	{
		printf("ERROR reading from file");
		sprintf(metaBuffer, "%d,%d,%d,%d", FAIL, -1, errno, h_errno);
		return metaBuffer;
	}
	sprintf(metaBuffer, "%d,%d,%s", SUCCESS, numRead, buffer);
	//printf("%d %s\n", numRead, buffer); 
	return metaBuffer;	
	
}
char* myWrite (clientData* userProfile)
{
	//printf("string: %s\n", userProfile -> writeString);
	char* buffer = (char*)malloc(sizeof(char)*100);
	bzero(buffer, 100);
	if (!checkPermissions(userProfile))
	{
		sprintf(buffer, "%d,%d,%d", FAIL, EACCES, h_errno);
		return buffer;
	}
	pthread_mutex_lock(&fileTableMutex);
	int numWritten = write(userProfile ->serverFD, userProfile->writeString, userProfile -> numBytes);
	pthread_mutex_unlock(&fileTableMutex);
	if (numWritten < 0)
	{
		printf("error writing  %s file descriptor = %d\n", strerror(errno), (ssize_t)userProfile ->serverFD);
		sprintf(buffer, "%d,%d,%d", FAIL, errno, h_errno); 
	}
	else
	{
		printf("NUM WRITTEN: %d\n", numWritten);	
		sprintf(buffer, "%d,%d,%d", SUCCESS, numWritten, errno);
	}
	printf("buffer: %s\n", buffer);
	return buffer;
}
char* myClose(clientData* userProfile)
{
	int serverFD = userProfile ->serverFD;;
	int filedes = -1;
	clientData *prev, *curr;
	int closeResult;
	char* buffer = (char*)malloc(sizeof(char)*100);;
	bzero(buffer, 100);
	int hashIndex = hash(serverFD);
	curr = fileTable->files [hashIndex];
	
	while (curr!= NULL && curr->serverFD!=serverFD)
	{
		prev = curr;
		curr = curr->next;
	}
	//if file is not opened
	if (curr == NULL)
	{
		sprintf(buffer, "%d,%d", FAIL, EBADF, h_errno);
		return buffer;
	}
	prev->next = curr->next;	
	closeResult = close(serverFD);
	if (closeResult < 0)
	{
		sprintf(buffer, "%d%d", FAIL, errno, h_errno);
		return buffer;
	}
	sprintf(buffer, "%d%d", SUCCESS, errno, h_errno);
	return buffer;
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
	int portno = 91128;
	int clilen = -1;
	int amtData = -1;
	char buffer [5000];
	struct sockaddr_in serverAddressInfo;
	struct sockaddr_in clientAddressInfo;
	pthread_mutex_intit(&fileTableMutex, NULL);

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







