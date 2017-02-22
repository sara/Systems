#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define malloc(x) mymalloc (x, __FILE__, __LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)

typedef enum {FALSE, TRUE} boolean;

//malloc array - holds metadata as a short and spaces allocated for user data
static char myBlock [5000];

//returns pointer to block of memory if sufficient space remains in myBlock
void* myMalloc(int numRequested);

//returns pointer to index of first contiguous block of suitable size if exists, -1 if doesn't
unsigned short* checkContiguous(char* myBlock, int numRequested);

//merges contiguous blocks of free memory into a single large block 
void defrag(char* myBlock);

int myFree (char* p);

void initArray(char* myBlock);
