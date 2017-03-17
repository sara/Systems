#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "tokenizer.h"


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

//strtok does append null terminator to each token it returns
//create a linked list where each node represents one token
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
		//skip all non-alphanumeric garbage
		while (c!=EOF && !isalnum(c))
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
//works for determined edge cases. will run more scenarios

int checkInput(int argc)
{
    //too few or too many inputs
	//printf("%i\n", argc);
	if(argc!=3)
        {
		   printf("usage: pointersorter.c output_file target_file \n");
            return 1;
        }
    return 0;
}
hashTable* makeMasterTable(recordNode* list, char* fileName)
{
	int count =0;
    //slot in the hashTable according to leading letter
    int index;
    //leading letter
    char leading;
    hashTable* hTable = makeHashTable(36);
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
		if (hTable->table[index] == NULL || sortalnum(hTable->table[index]->token, node->token)<0)    
			{
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
					curr -> count ++;
				}
				else
                {
					node->next = curr;
                	prev->next = node;
            	}
			}
			list = list->next;
			
	}
    return hTable;
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
			
			if (curr!= NULL)
			{
				recordNode* temp = makeNode(curr->fileName, curr->token);
				temp -> count = curr->count;
				temp -> next = curr -> next;
				masterTable -> table[i] = temp;
			}
			else
			{
				masterTable -> table[i] = NULL;
			}
			prev->next = NULL;
			if(head!=NULL)
			{
				scatterTokens(head, maxNum, outputFile);
			}
			head = masterTable->table[i];
		}
	}
    closeOutput(outputFile);
}
//I'm like 99% sure this works
hashTable* scatterTokens (recordNode* head, int size, FILE* outputFile)
{
	recordNode *curr, *prev;
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
			while(curr!=NULL && sortalnum(curr->fileName, head->fileName)>0)
			{
				prev = curr;
				curr = curr->next;
			}
			//if (prev == NULL)
			//{
			//	printf("null prev\n");
			//}
			recordNode* temp = makeNode(head->fileName, head->token);
			temp->count = head->count;
			myTable->table[head->count-1] = temp;
		}
		head = head->next;
	}
	outputTokenList(myTable, outputFile);
	
}
//char* outputFile
void outputTokenList (hashTable* myTable, FILE* outputFile)
{
	boolean wordInitialized = FALSE;
	if (!outputInitialized)
	{
		initializeOutput(outputFile);
		outputInitialized = TRUE;
	}
	int i;
	recordNode* curr, prev;
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
		//	while(sortalnum(currTok, curr->token)==0)
		//	{
				fprintf(outputFile, "\t\t<file name = \"%s\">%i</file>\n",curr->fileName, curr->count);
				curr = curr-> next;
		//	}
			fprintf(outputFile, "\t</word>\n");
		}
	}
}
//not sure if i have to pass argv here or not...feel like no
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
    for (i=0; i<52; i++)
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
		printf("%s\n", head->token);
		head = head->next;
	}
}

recordNode* otherTokenize(char* inputString, char* fileName)
{
    recordNode* head = NULL;
    char buffer [5000];
    int index = 0;
	int stringIndex=0;
	recordNode* curr = NULL;

	while (inputString[stringIndex] != '\0')
    {
		index = 0;
		//skip all non-alphanumeric garbage
		while (inputString[stringIndex]!='\0' && !isalnum(inputString[stringIndex]))
		{
			stringIndex++;
		}
		//get all alphanumerics in current token
		while(inputString[stringIndex]!= '\0' && isalnum(inputString[stringIndex]))
		{
			buffer[index] = inputString[stringIndex]; //does this move the pointer every time?
			index ++;
			stringIndex++;
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

int main (int argc, char** argv)
{
	boolean outputInitialized = FALSE;
	
	FILE* inputFile = fopen(argv[2], "r");
	if(checkInput(argc) == 1)
	{
		return 1;
	}
	recordNode* head = tokenize(inputFile, argv[2]);
	hashTable* myTable = makeMasterTable(head, argv[1]);
    FILE* outputFile = fopen(argv[1], "w+");
	outputTokens(myTable, outputFile);
//    fclose(inputFile);
//    fclose(outputFile);
	return 0;
}

