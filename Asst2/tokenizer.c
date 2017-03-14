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
	printf("%i\n", argc);
	if(argc!=3)
        {
		   printf("usage: pointersorter.c output_file target_file \n");
            return 1;
        }
    return 0;
}
hashTable* makeMasterTable(recordNode* list, int size, char* fileName)
{
	int count =0;
    //slot in the hashTable according to leading letter
    int index;
    //leading letter
    char leading;
    hashTable* hTable = makeHashTable(size);
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
				printf("first: %s\n", node->token); 
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
					printf("DUPLICATE\n");
					curr -> count ++;
				}
				else
                {
					printf("second: %s\n", node->token);
					node->next = curr;
                	prev->next = node;
            	}
			}
			list = list->next;
			
	}
	printf("COUNT: %i\n", count);
    return hTable;
}

//collects tokens, scatters into individual hash tables, and outputs them to designated output file
void outputTokens(hashTable* masterTable, char* outputFile)
{
	int i;
	recordNode* head, *curr, *prev;
	char* currTok;
	int maxNum;
	//traverse entire master hash Table
	for (i=0; i<sizeof(masterTable)/sizeof(recordNode); i++)
	{	
		head = masterTable->table[i];
		//collect all different token lists from each index
		while (masterTable->table[i]!=NULL)
		{
			curr = masterTable->table[i];
			prev = masterTable->table[i];
			currTok = masterTable->table[i]->token;
			maxNum = curr->count;
			//keep traveling down the line as long as the token is the same
			while (sortalnum(currTok, curr->token)==0)
			{
				if (curr->count>maxNum)
					{
						maxNum = curr->count;
					}
				prev = curr;
				curr = curr->next;
			}
			//cut list when you hit a new token; move the next stream of tokens in the same index up to the top of the index
			prev->next = NULL;
			masterTable->table[i] = curr;
			scatterTokens(head, maxNum, outputFile);
		}
	}
}

hashTable* scatterTokens (recordNode* head, int size, char* outputFile)
{
	recordNode *curr, *prev;
	hashTable* myTable = makeHashTable(size);
	while (head!=NULL)
	{
		if(myTable->table[head->count-1]!=NULL)
		{
			myTable->table[head->count-1] = head;
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
			prev -> next = head;
			head = head->next;
		}
	}
	outputTokenList(myTable, outputFile);
}

void outputTokenList (hashTable* myTable, char* outputFile)
{
	if (!outputInitialized)
	{
		initializeOutput(outputFile);
		outputInitialized = TRUE;
	}
	int i;
	recordNode* curr, prev;
	char* currTok;
	FILE* file = fopen(outputFile, "w+");
	boolean wordInitialized = FALSE;
	for (i=0; i<sizeof(myTable)/sizeof(recordNode*); i++)
	{
		curr = myTable->table[i];
		currTok = curr->token;
		while(curr!=NULL)
		{
			fprintf(file, "\t<word text = \"%s\">\n", curr->token);
			while(sortalnum(currTok, curr->token)==0)
			{
				fprintf(file, "\t\t<file name = \"%s\">%i</file>\n",curr->fileName, curr->count);
				curr = curr-> next;
			}
			fprintf(file, "\t</word>\n");
		}
	}
	closeOutput(outputFile);
}
//not sure if i have to pass argv here or not...feel like no
void initializeOutput(char* outputFile)
{
	FILE* file = fopen(outputFile,"w+");
	fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", file);
	fputs("<fileIndex>\n", file);
	fclose(file);
}

void closeOutput(char* outputFile)
{
	FILE* file = fopen(outputFile, "w+");
	fputs("</fileIndex>", file);
	fclose(file);
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
	
	FILE* fp = fopen(argv[2], "r");
	/*if(checkInput(argc) == 1)
	{
		return 1;
	}*/
	//recordNode* head = tokenize(fp, argv[2]);
	recordNode* head = otherTokenize(inputString, "test.txt");
	hashTable* myTable = makeMasterTable(head, 36, "test.txt");
	printTable(myTable);
/*	printf("completed\n");
	//outputTokens(myTable, argv[1]);
	*/
	return 0;
}

