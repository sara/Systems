#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef struct Node
{
	char* fileName;
	int count;
	char* token;
	struct Node* next;
}recordNode;

typedef struct HashTable
{
	recordNode** table;
	int length;
}hashTable;

typedef enum {FALSE, TRUE} boolean;

char* inputString = "this is a test sentence now^Im utilizing $%^&symbols$%^&*(*&^$as+_)(*&delimiters This  o3ne has9876568798 nu289374mbers\0";

boolean outputInitialized = FALSE;

recordNode* makeNode(char* fileName, char* token);

hashTable* makeHashTable(int size);

recordNode* tokenize(FILE* file, char* fileName);

int checkInput(int argc);

hashTable* makeMasterTable(recordNode* head, int size, char* fileName);

void outputTokens(hashTable* masterTable, char* outputFile);

hashTable* scatterTokens(recordNode* head, int size, char* outputFile);

void outputTokenList(hashTable* myTable, char* outputFile);

void initializeOutput();

void closeOutput();

void myToLower(recordNode* head);

int sortalnum(const char* a, const char* b);

void printTable(hashTable* hTable);

void destroyTable(hashTable* hTable);

void destroyList(recordNode* head);

void printLL(recordNode* head);

