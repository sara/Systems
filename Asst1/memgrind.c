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
        for(i = 0; i<1; i++)
		{
                myPointers[i] = (char*)malloc(1*sizeof(char));
        }
        for(i = 0; i<1; i++)
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
                tmp = (char*)malloc(1*sizeof(char));
                free(tmp);
		}
}
//randomly malloc or free 1000 times, making sure to malloc 1000 times and free all pointers
void grindC()
{
	char* myPointers[1000];
	int numFrees = 0;
	int numMallocs = 0;
	int numSaved = 0;
	int arrIndex = 0;
	int freeIndex = 0;
	int op, i;
	initMyPointers(myPointers);

	for(i=0; i<1000; i++)
	{
		printf("i = %i\n",i);
		op = rand();
		if(numSaved == 0)
			op = 0;
		
		switch(op%2)
		{
			case 0:
			numMallocs++;
			printf("arrIndex: %i\n", arrIndex);
			myPointers[arrIndex] = (char*)malloc(sizeof(char));
			arrIndex ++;
			numSaved++;
			break;

			case 1:
			printf("arrIndex: %i\n", arrIndex);
			numFrees++;
			free(myPointers[freeIndex]);
			freeIndex++;
			numSaved --;
			break;

			default:
			printf("ERROR IN GRINDC\n");
		}
	}
		while(numFrees<1000)
		{
			op = rand();
			if (op%2 == 0 && numMallocs == 1000)
				op = 1;
			else if (op%2 == 1 && numSaved ==0)
				op =0;
			if (op%2==1)
			{
				numFrees++;
				free(myPointers[freeIndex]);
				freeIndex ++;
				numSaved --;
			}
			else
			{
				numMallocs ++;
				myPointers[arrIndex] = (char*)malloc(sizeof(char));
				arrIndex++;
				numSaved ++;
			}
		}
	printf("arrIndex = %i, freeIndex = %i\n", arrIndex, freeIndex);
}

void grindD()
{
	char* myPointers [1000];
	initMyPointers(myPointers);
	int size, operation;
	int index = 0;
	int j;
	int numMallocs = 0;
	int numFrees = 0;
	int spaceUsed = 2;
	while (numMallocs < 1000)
	{
		//odd indicates malloc, even indicates free
		operation = (rand()+1) % 2;
		//if malloc
		if (operation == 1 && spaceUsed < 5000)
		{
			numMallocs ++;
			//generate a pointer between 1 and 65 bytes
			size = (rand() %65) + 1;
			char* p = (char*)malloc(size*sizeof(char));
			if (size%2 == 1)
				size++;
			myPointers[index] = p;
			index ++;
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
			if (sizeof(myPointers[j])%2 == 1)
				spaceUsed --;
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
	printf("final space used: %i\n", spaceUsed); 
}
//start at mallocing 5000, then decrement by two, alternating malloc and free each time

void grindE()
{
	
}


void main(int argc, char * argv[])
{
//	grindA();
//	grindB();
	grindC();
//	grindD();
//  int x;
//  free(&x);
}

