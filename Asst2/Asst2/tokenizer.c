#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include "tokenizer.h"

recordNode* tokenize(FILE* file, char* fileName)
{
    recordNode* head = NULL;
    char buffer [5000];
    int index = 0;
	recordNode* curr = NULL;
	char c = fgetc(file);
	while (c != EOF)
    {
		index = 0;
		//skip all non-alphanumeric garbage THIS NEEDS TO SKIP ON FIRST NUMBERS TOO APPARENTLY
		while (c!=EOF && !isalpha(c))
		{
			c = fgetc(file);
		}
		//get all alphanumerics in current token
		while(c!= EOF && isalnum(c))
		{
			buffer[index] = c; //does this move the pointer every time?
			index ++;
			c = fgetc(file);
		}
		//make sure string is null terminated
		buffer[index] = '\0';
		char* currTok = malloc(sizeof(char)*index);
		if(strlen(buffer)>0)
        {
		//+1?
			memcpy(currTok, buffer, strlen(buffer));
			if(head == NULL)
       	 	{
            	head = makeNode(fileName, currTok);
            	curr = head;
        	}
        	else
        	{
            	curr->next = makeNode(fileName, currTok);
            	curr = curr-> next;
        	}
    	}
	}
	myToLower(head);
    return head;
}


void travdir (hashTable* myTable, const char * dir_name)
{

	DIR * dir;
	FILE* targetFile;

	//printf("%s\t %d\n", dir_name, sizeof(dir_name));
	dir = opendir(dir_name);
	if(!dir)
	{
		if (errno == ENOTDIR)
		{
			char buffer [256];
			memcpy (buffer, dir_name, sizeof(dir_name)+1);
			buffer[sizeof(dir_name)+1] = '\0';
			targetFile = fopen(dir_name, "r");
			if (targetFile == NULL)
			{
				printf("file is null\n");
			}
			//printf("%s\n", buffer);
			recordNode* tokenStream = tokenize(targetFile, buffer);
			addToTable(tokenStream, myTable, buffer);
			return;
		}
		else
		{
			printf("Error: could not open %s - File or directory  may not exist\n ", dir_name);
				return;
		}
	}
	while(dir !=NULL)
	{

		struct dirent * entry;
		char * d_name;
		entry = readdir(dir);
		if(!entry)
		{
			//end of stream, break
			break;
		}
		d_name = entry->d_name;
		
		switch(entry->d_type)
		{
			
			case DT_DIR:
			{
				if(strcmp(d_name,".") != 0 && strcmp(d_name, "..") != 0)
				{
					//need to EXTEND THE PATH for next travdir call, working dir doesn't change (think adir/ -> adir/bdir/....)
					int pathlength = 0;	
					char path[256];
					pathlength = snprintf(path, 256, "%s/%s",dir_name, d_name);
					if(pathlength > 255)
					{
						printf("Path length is too long error");
						return;
					}
					//strcat(path, d_name); //lengthens path 
					//printf("%s\n",d_name); //error checking and DEBUGGING
					travdir(myTable, path); //RECURSIVE STEP
				}
				break;
			}
			case DT_REG:
				//regular files, need to check to ensure ".txt"....
			{	
				char pathname [256];
				FILE* targetFile;
				sprintf(pathname, "%s/%s", dir_name, d_name);
				targetFile = fopen(pathname, "r");
				if (targetFile!=NULL)
				{
						recordNode* tmp = tokenize(targetFile, d_name);	//  <-----------------------------HERE IS THE TOKENIZE CALL
					addToTable(tmp, myTable, d_name);
				}
				break;
			}
			default:
				printf("something is not right in ur switch statement");
				return;
		}
	
	}
//	printf("closing directory: %s\n", dir_name); //DEBUGGING 
	if(closedir(dir)){
		printf("error could not close dir");
		return;
	}
}

recordNode* makeNode(char* fileName, char* token)
{
    recordNode* myNode = (recordNode*)calloc(1, sizeof(recordNode));
    //mallocs and copies data into new string
    myNode -> fileName = strdup(fileName);
    myNode -> count = 1;
    myNode -> token = token;
    myNode -> next = NULL;
    return myNode;
}

hashTable* makeHashTable(int size)
{
    hashTable* myTable = (hashTable*)malloc(sizeof(hashTable));
    myTable -> table = (recordNode**)malloc(sizeof(recordNode*)*size);
    int i;
    for (i=0; i<size; i++)
    {
        myTable->table[i] = NULL;
    }
    myTable->length = size;
    return myTable;
}

int checkInput(int argc)
{
    //too few or too many inputs
	//printf("%i\n", argc);
	if(argc!=3)
        {
		   printf("usage: pointersorter.c output_file target_file/directory \n");
            return 1;
        }
    return 0;
}

//collects tokens, scatters into individual hash tables, and outputs them to designated output file
void outputTokens(hashTable* masterTable, FILE* outputFile)
{
	int i;
	recordNode* head;
	recordNode* curr;
	recordNode* prev;
	char* currTok;
	int maxNum;
	
	for (i=0; i<masterTable->length; i++)
	{
		head = masterTable->table[i];
		while(head!=NULL)
		{
			curr = head;
			prev = curr;
			currTok = head->token;
			maxNum = curr->count;

			while (curr!=NULL && sortalnum(currTok, curr->token) ==0)
			{
				if(curr->count > maxNum)
				{
					maxNum = curr->count;
				}
				prev = curr;
				curr = curr->next;
			}
			
				masterTable -> table[i] = curr;
			prev->next = NULL;
			if(head!=NULL)
			{
				scatterTokens(head, maxNum, outputFile);
			}
			head = masterTable->table[i];
		}
	}
	//destroyTable(masterTable);
    closeOutput(outputFile);
}
//I'm like 99% sure this works
void scatterTokens (recordNode* head, int size, FILE* outputFile)
{	
	recordNode *curr, *prev;//, *toFree;
//	toFree = head;
	hashTable* myTable = makeHashTable(size);
	while (head!=NULL)
	{
		if(myTable->table[head->count-1]==NULL)
		{
			recordNode* temp = makeNode(head->fileName, head->token);
			temp->count = head -> count;
			myTable->table[head->count-1] = temp;
		}
		else
		{
			curr = myTable->table[head->count-1];
			prev = curr;
			//for the same token with the same counts for different files, keep alphanumeric order
			while(curr!=NULL && strcmp(curr->fileName, head->fileName)<0)//sortalnum(curr->fileName, head->fileName)>0)
			{
				prev = curr;
				curr = curr->next;
			}
			recordNode* temp = makeNode(head->fileName, head->token);
			temp->count = head->count;
			temp->next = curr;
			if (myTable->table[temp->count-1] == curr)	
			{
				myTable->table[temp->count-1] = temp;
			}
			else
			{
				prev->next = temp;
			}
	
		}
		head = head->next;
	}
	//destroyList(toFree);
	outputTokenList(myTable, outputFile);
}

void outputTokenList (hashTable* myTable, FILE* outputFile)
{
	boolean wordInitialized = FALSE;
	if (!outputInitialized)
	{
		initializeOutput(outputFile);
		outputInitialized = TRUE;
	}
	int i;
	recordNode* curr;
	for (i=0; i<myTable->length; i++)
	{
		curr = myTable->table[i];
		while(curr!=NULL)
		{
			if(!wordInitialized)
			{
				fprintf(outputFile, "\t<word text = \"%s\">\n", curr->token);
				wordInitialized = TRUE;
			}
			//while(sortalnum(currTok, curr->token)==0)
			while (curr!=NULL)
			{
				fprintf(outputFile, "\t\t<file name = \"%s\">%i</file>\n",curr->fileName, curr->count);
				curr = curr-> next;
			}
		}
	}
	fprintf(outputFile, "\t</word>\n");
	//destroyTable(myTable);
}
void initializeOutput(FILE* outputFile)
{
	fprintf(outputFile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(outputFile, "<fileIndex>\n");
}

void closeOutput(FILE* outputFile)
{
	fprintf(outputFile, "</fileIndex>");
}
void myToLower(recordNode* head)
{
	int i;
	recordNode* temp = head;
	while(temp!=NULL)
	{	
		for (i=0; i<strlen(temp->token); i++)
		{
			if (isupper(temp->token[i]))
			{
				temp->token[i] = tolower(temp->token[i]);
			}
		}
		temp = temp->next;
	}
}

//modified strncmp to allow for alphanumerics
int sortalnum(const char *a, const char *b)
{
	while(*a != '\0' && *b != '\0')
		{
    
        	if (*a == *b)
			{
        	    a++;
        	    b++;
        	    continue;
        	}
        
        	if( (isalpha(*a) && isalpha(*b)) || (isdigit(*a) && isdigit(*b)) )
			{
            
        		if(*a < *b)
				return 1;
                
            		if(*b < *a)
                		return -1;
        	}
        	else if(isalpha(*a) && isdigit(*b))
			{
            		return 1;
        	}    
        	else
			{       //should be isdigit(*a) && isalpha(*b)
            		return -1;
        	}    
    	}
	//both pointers have matched until one hits null terminator
	if(*a == '\0' && *b == '\0')
	{
	//both pointers matched and are same length
		return 0;
	}
	else if(strlen(a) < strlen(b)){
	//first string is shorter
		return 1;
	}	
	else
	{
	//first string is longer
		return -1;
	}
}

void printTable(hashTable* hTable)
{
    int i;
    recordNode* curr;
    for (i=0; i<hTable->length; i++)
    {
        curr = hTable->table[i];
		while(curr!=NULL)
        {
            printf("token: %s   count: %i   fileName: %s\n", curr->token, curr->count, curr-> fileName);
            curr = curr->next;
        }
    }
}

//free all inner nodes and table itself
void destroyTable(hashTable* hTable)
{
    int i;
    recordNode* curr;
    recordNode* temp;
	for (i=0; i<hTable->length; i++)
    {
        curr = hTable->table[i];
        while(curr!=NULL)
        {
            temp = curr->next;
            free(curr->token);
			free(curr->fileName);
            free(curr);
            curr = temp;   		
		}
    }
    free(hTable->table);
    free(hTable);
}
//free unsorted temp linked list
void destroyList(recordNode* head)
{
    recordNode* temp;
	while(head!=NULL)
    {
        temp = head->next;
        free(head->token);
		free (head->fileName);
        free(head);
        head = temp;
    }

}

void printLL(recordNode* head)
{
	//recordNode* ptr = head;
	while(head != NULL)
	{
		printf("%s, %s\n", head->token, head ->fileName);
		head = head->next;
	}
}

int checkOverwrite(char** argv)
{
	int x = 1;
	char file [260];
	file[0] = '.';
	file[1] = '/';
	memcpy(file+2, argv[1], strlen(argv[1])+1);
	printf("%s\n", file);
	if(access(file, F_OK) == 0)
	{
		printf("File already exists in directory. Do you wish you overwrite it? Enter 1 to proceed or 0 to exit\n");
	x = getchar();
	}
	return x;
}


void addToTable(recordNode* list, hashTable* hTable , char* fileName)
{
	
	int count =0;
    //slot in the hashTable according to leading letter
    int index;
    //leading letter
    char leading;
    //hashTable* hTable = makeHashTable(36);
	while(list!=NULL)
	{
		count ++;
		leading = list->token[0];
        	index = leading;
		//alphas first in table, numerics second
		if (!isalpha(leading))
			{
				index += 26;
			}
		else
			{
				index -=97;
			}
		//node to be inserted
        recordNode* node = makeNode(fileName, list->token);
        //if node is to be inserted at front of list
		if (hTable->table[index] == NULL || sortalnum(hTable->table[index]->token, node->token)<0)    		{
            node->next = hTable->table[index];
            hTable->table[index] = node;
		}
        //if node is second node or later
            else
            {
                recordNode* curr = hTable->table[index];
                recordNode* prev = curr;
            //while string to be inserted comes after existing strings
                while(curr!=NULL && sortalnum(curr->token, node->token)>0)
                {
                    prev = curr;
                    curr = curr->next;
                }
			if (curr!=NULL && sortalnum(curr->token, node->token)==0)
			{
				if (strcmp(curr->fileName, node->fileName)!=0)
				{
								//HERE BEGINS THE NEW TERRITORY
							if(curr->next != NULL && strcmp(curr->next->token, node->token)==0 && strcmp(node->fileName, curr->next->fileName)==0)
							{
								//printf("well it might be working");	
								curr->next->count++;
							}
							else
							{
								node->next = curr->next;
								curr->next=node;
							}
				}
							else
							{
								curr->count ++;
							}
				}
				else
				{
					node->next = curr;
					prev->next = node;
				}
			}
			list = list->next;
			}
			return;
	}


int main (int argc, char** argv)
{
	hashTable* myTable = makeHashTable(36);

	if(checkInput(argc) == 1)
	{
		return 1; //-1?
	}
	if (checkOverwrite(argv)==0)
	{
		return 0;
	}
	travdir(myTable, argv[2]);
	FILE* outputFile = fopen(argv[1], "w+");
	outputTokens(myTable, outputFile);
	return 0;
}


