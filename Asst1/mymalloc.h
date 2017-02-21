#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef enum {FALSE, TRUE} boolean;

//malloc array - holds metadata as a short and spaces allocated for user data
static char myBlock [5000];

//returns pointer to block of memory if sufficient space remains in myBlock
char* myMalloc(int numRequested);

//returns pointer to index of first contiguous block of suitable size if exists, -1 if doesn't
short* checkContiguous(char* myBlock, int numRequested);

void defrag(char* myBlock);

int myFree (char* p);
