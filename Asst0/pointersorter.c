#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


//modify to return error codes (i.e. check if malloc/calloc works properly, exit nicely if not)
typedef struct node
{
	char* data;
	struct node* next;
}Node;

typedef struct HashTable
{
	Node** table;
	int numFilled;
}hashTable;

Node* makeNode(char* data)
{
	Node* myNode = (Node*)calloc(1, sizeof(Node));
	//mallocs and copies data into new string
	myNode -> data = strdup(data);
	myNode -> next = NULL;
	return myNode;
}
//strtok does append null terminator to each token it returns
//create a linked list where each node represents one token
Node* tokenize(char* string)
{
	Node* head = NULL;
	//current node to attach new additions
	Node* mover = NULL;
	int i, start, end, size;
	for (i=0; i<strlen(string); i++)
	{
		//get start index of alphabetic characters
		start = i;
		while(!isalpha(string[i]))
		{
			start++;
			i++;
		}
		//get end index of alphabetic characters
		end = start+1;
		while(isalpha(string[end]))
		{
			i++;
			end++;
		}
		size = end-start+1;
		//copy relevant characters over into temporary string
		char temp[size];
		memcpy(temp, string+start, size);
		//make sure temporary string is properly null terminated
		temp[size-1] = '\0';
		Node* curr = makeNode(temp);
		//initialize list if necessary
		if(head == NULL)
			{
				head = curr;
				mover = head;
			}
		//add on to existing list otherwise
		else
			{
				mover->next = curr;
				mover = mover->next;
			}
	}
	
	return head;
}
hashTable* makeHashTable()
{
	hashTable* myTable = (hashTable*)malloc(sizeof(hashTable));
	//not entirely sure if this is a double pointer. i'm pretty sure it is though
	myTable -> table = (Node**)malloc(sizeof(Node*)*52);
	int i;
	for (i=0; i<52; i++)
	{
		myTable->table[i] = NULL;
	}
	myTable->numFilled = 0;
	return myTable;
}
//works for determined edge cases. will run more scenarios
int checkInput(int argc, char** argv)
{
	char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	//too few or too many inputs
	if(argc!=2)
		{
			printf("usage: pointersorter.c input_string\n");
			return 1;
		}
	//string is empty or contains no alphas	
	if (strlen(argv[1]) == 0 || !strpbrk(argv[1], alphabet))
	{
		return 1;
	}
	return 0;

}
hashTable* sort(Node* head)
{
	//slot in the hashTable according to leading letter
	int index;
	//leading letter
	int leading;
	hashTable* hTable = makeHashTable();
	//insert each Node into hashTable, staggering upper and lower cases for intuitive ordering
	while(head!=NULL)
	{
		leading = head->data[0];
		//uppercase
		if(leading<=90)
			index = 2*(leading % 65);
		//lowercase
		else
			index = 2*(leading-96)-1;
		//node to be inserted
		Node* node = makeNode(head->data);
		//if node is to be inserted at front of list
		if(hTable->table[index]==NULL || strcmp(hTable->table[index]->data, node->data)>=0)
			{
				node->next = hTable->table[index];
				hTable->table[index] = node;
			}
		//if node is second node or later
			else
			{
				Node* curr = hTable->table[index];
				Node* prev = hTable->table[index];
			//while string to be inserted comes after existing strings
				while(curr!=NULL && strcmp(curr->data, node->data)<0)
				{	
					prev = curr;
					curr = curr->next;
				}
				node->next = curr;
				prev->next = node;
			}
			head = head->next;
	}
	return hTable;
}

void printTable(hashTable* hTable)
{
	int i;
	Node* curr;
	for (i=0; i<52; i++)
	{
		curr = hTable->table[i];
		while(curr!=NULL)
		{
			printf("%s\n", curr->data);
			curr = curr->next;
		}
	}
}
//free all inner nodes and table itself
void destroyTable(hashTable* hTable)
{
	int i;
	Node* curr;
	Node* temp;
	for (i=0; i<52; i++)
	{
		curr = hTable->table[i];
		while(curr!=NULL)
		{
			temp = curr->next;
			free(curr->data);
			free(curr);
			curr = temp;
		}
	}
	free(hTable->table);
	free(hTable);
}
//free unsorted temp linked list
void destroyList(Node* head)
{
	Node* temp;
	while(head!=NULL)
	{
		temp = head->next;
		free(head->data);
		free(head);
		head = temp;
	}
}
void printStringList(Node* head)
{
	while(head!=NULL)
	{
		printf("%s\n", head->data);
		head = head->next;
	}
}



int main(int argc, char** argv)
{
	//exit if invalid input
	if (checkInput(argc, argv) == 1)
		return 1;
	char* inputString = argv[1];
	Node* stringList = tokenize(inputString);		
	hashTable* myTable = sort(stringList);
	printTable(myTable);
	destroyTable(myTable);
	destroyList(stringList);
	return 0;
}

