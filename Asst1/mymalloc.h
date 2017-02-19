//h
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum boolean {FALSE, TRUE};

//malloc array - holds metadata as a short and spaces allocated for user data
static char myBlock [5000];

//returns pointer to block of memory if sufficient space remains in myBlock
char* myMalloc(int numRequested);


//returns index of first contiguous block of suitable size if exists, -1 if doesn't
int checkContiguous(char* myBlock, int numRequested);


