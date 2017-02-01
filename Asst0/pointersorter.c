#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

//transform all nonalpha characters to spaces so strtok can be used with a single delimiter
char* parseString(char* string)
{
	//note* strlen doesn't counts up to but not including the null terminator
	int i;
	for (i=0; i<strlen(string); i++)
	{
		if (!isalpha(string[i]))
		{
			string[i] = ' ';
		}
	}

	return string;
}
Node* makeNode(char* data)
{
	Node* myNode = (Node*)calloc(sizeof(Node));
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
	const char delim [2] = " ";
	char* curr;
	for (curr = strtok(string, delim); curr!=NULL; curr = strtok(NULL, delim))
	{
		//continuously add nodes to front of linked list
		Node* temp = makeNode(curr);
		temp->next = head;
		head = temp;
	}
	return head;
}

hashTable* makeHashTable()
{
	hashTable* myTable = (hashTable*)calloc(sizeof(hashTable));
	//not entirely sure if this is a double pointer. i'm pretty sure it is though
	myTable -> table = (Node**)calloc(sizeof(Node*)*52);
	myTable -> numFilled = 0;
	return myTable;
}

//works for determined edge cases. will run more scenarios
int checkInput(int argc, char** argv)
{
	char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	//too few or too many inputs
	if(argc!=2)
		{
			printf("usage: pointersorter.c input_string");
			return 1;
		}
	//string is empty or contains no alphas	
	if (strlen(argv[1]) == 0 || !strpbrk(argv[1], alphabet))
	{
		return 1;
	}
	return 0;
}

void sort(Node* head)
{
	//slot in the hashTable according to leading letter
	int index;
	//leading letter
	int leading;
	//ordering rank
	int rank;
	hashTable* hTable = (hashTable*)malloc(sizeof(hashTable));
	//insert each Node into hashTable, staggering upper and lower cases for intuitive ordering
	while(head!=NULL)
	{
		leading = head->data[0];
		//uppercase
		if(leading<=90)
			index = 2*(leading - 65);
		//lowercase
		else
			index = 2*(leading-97)-1;
		//node to be inserted
		Node* node = makeNode(head.data);
		//node already at the desired index
		Node* curr = hTable->table[index];
		Node* prev = hTable->table[index];
		//if node is to be inserted at front of list
		if(hTable[index]==NULL || strcmp(curr->data, node->data)<=0)
			{
				node->next = curr;
				hTable->table[index] = node;
			}
		//if node is second node or later
			else
			{
			//while string to be inserted comes after existing strings
				while(curr!=null && strcmp(curr->data, node->data)>0)
				{	
					else
					{
						prev = curr;
						curr = curr.next;
					}
				
				}
				node.next = curr;
				prev.next = node;
			}
	}
}


void printTable(hashTable* hTable)
{
	int i;
	Node curr;
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
	Node curr, temp;
	for (i=0; i<52; i++)
	{
		curr = hTable->table[i];
		while(curr!=NULL)
		{
			temp = curr->next;
			free(curr);
			curr = temp;
		}
	}
	free(hTable);
}
//free unsorted temp linked list
void destroyList(Node* head)
{
	Node temp;
	while(head!=NULL)
	{
		temp = head.next;
		free(head);
		head = temp;
	}
}
int main(int argc, char** argv)
{
	//exit if invalid input
	if (checkInput(arc, argv) == 1)
		return 1;
	char*
	
	
	return 0;
}


