#include "mymalloc.c"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


void initMyPointers(char** pointerArr)
{
	int i;
	for (i=0; i<sizeof(pointerArr)/sizeof(char*); i++)
	{
		pointerArr[i] = NULL;
	}
}

void grindA()
{
        char * myPointers[1000];
		initMyPointers(myPointers);
        int i = 0;
        for(i = 0; i<1000; i++)
		{
                myPointers[i] = (char*)malloc(1*sizeof(char));
        }
        for(i = 0; i<1000; i++)
		{
                free(myPointers[i]);
        }
}

void grindB()
{

        char * tmp = NULL;
        int j = 0;
        for(j = 0; j<1000; j++)
		{
                char * tmp = (char*)malloc(1*sizeof(char));
                free(tmp);
		}
}

void grindC()
{
	char* myPointers[1000];
	initMyPointers (myPointers);
	int operation;
	int countmalloc = 0;
	int position = 0;
	int counter = 1000;

	while (counter>0)
	{
		if(countmalloc >= counter)
		{
			operation = 1;
		}
		else if (countmalloc == 0)
		{
			operation = 0;
		}
		else
		{
			operation = rand()%2;
		}

		switch(operation)
		{
			case 0:
			{
				countmalloc ++;
				myPointers[position] = (char*) malloc (1*sizeof(char));
				position++;
				break;
			}
			case 1:
			{
				position --;
				free(myPointers[position]);
				countmalloc --;
				break;
			}
			default:
			{
				printf("error");
				return;
			}
			counter --;
		}
	}
}

void grindD()
{
	char* myPointers [1000];
	initMyPointers(myPointers);
	int size, operation;
	int index = 0;
	int j;
	int numMallocs = 0;
	int spaceUsed = 2;
	while (numMallocs < 1000)
	{
		//odd indicates malloc, even indicates free
		operation = rand() % 2;
		//if malloc
		if (operation == 1 && spaceUsed < 5000)
		{
			numMallocs ++;
			//generate a pointer between 1 and 65 bytes
			size = (rand() %65) + 1;
			index ++;
			if (size%2==1)
				size++;
			//save the pointer in an array
			myPointers[index] = (char*)malloc(size*sizeof(char));
			spaceUsed += size+2;
		}
		else
		{
			j=0;
			while (myPointers[j] == NULL)
			{
				j++;
			}
			spaceUsed -= sizeof(myPointers[j]) +2;
			myPointers[j] = NULL;
		}
		printf("NumMallocs: %i, space used: %i\n", numMallocs, spaceUsed); 
	}
	j = 0;
	while (spaceUsed >0)
	{
		
		if (myPointers[j] != NULL)
		{
			spaceUsed -= *myPointers[j] + 2;
			myPointers[j] = NULL;
			j++;
		}
	}
}
//start at mallocing 5000, then decrement by two, alternating malloc and free each time

void grindE()
{
	
}


void main(int argc, char * argv[])
{
//	grindA();
//	grindB();
//	grindC();
	grindD();
}

