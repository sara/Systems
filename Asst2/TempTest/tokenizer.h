#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

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

void addToTable(recordNode* list, hashTable* hTable , char* fileName);
void travdir (hashTable* hTable, const char * dir_name);

recordNode* makeNode(char* fileName, char* token);

hashTable* makeHashTable(int size);

recordNode* tokenize(FILE* file, char* fileName);

int checkInput(int argc);

//recordNode* head, int size, char* fileName)

hashTable* makeMasterTable(recordNode* head, char* fileName);

//char* outputFile
void outputTokens(hashTable* masterTable, FILE* outputFile);

void scatterTokens(recordNode* head, int size, FILE* outputFile);

void outputTokenList(hashTable* myTable, FILE* outputFile);

void initializeOutput();

void closeOutput();

void myToLower(recordNode* head);

int sortalnum(const char* a, const char* b);

void printTable(hashTable* hTable);

void destroyTable(hashTable* hTable);

void destroyList(recordNode* head);

void printLL(recordNode* head);


