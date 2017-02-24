include "mymalloc.h"
include <time.h>
include <stdlib.h>
include <stdio.h>

void main(int argc, char * argv[])
{

}
void grindA()
{
        char * myPointers[1000] = NULL;
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
                char * tmp = (char*) malloc(1*sizeof(char));
                free(tmp);
        }
}

void grindC()
{

}

void grindD()
{
	
}

int grindD()
{
	char* myPointers [1000];
	initArr(myPointers);
	int size, operation;
	int index = 0;
	int j;
	int numMallocs = 0;
	int spaceUsed = 0;
	while (numMallocs < 1000)
	{
		operation = rand() % 2;
		//if malloc
		if (operation == 1 && spaceUsed < 5000)
		{
			numMallocs ++;
			size = (rand() %65) + 1;
			myPointers[index] = (char*)mymalloc(size*sizeof(char));
			index ++;
			spaceUsed += size+2;
		}
		else
		{
			j=0;
			while (myPointers[j] !=NULL)
			{
				j++;
			}
			spaceUsed -= *myPointers[j] +2;
			myPointers[j] = NULL;
		}
	}
	j = 0;
	while (spaceUsed >0)
	{
		if (myPointers[j] != NULL)
		{
			spaceUsed -= *myPointers[j] + 2;
			myPointers[j] = NULL;
		}
	}
}
//start at mallocing 5000, then decrement by two, alternating malloc and free each time

void initArray(char** pointerArr)
{
	int i;
	for (i=0; i<sizeof(pointerArr)/sizeof(char*); i++)
	{
		pointerArr[i] = NULL;
	}
}
