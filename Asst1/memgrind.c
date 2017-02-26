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

float grindA()
{
    struct timeval start, end;
	float times[100];
	int j;
	char * myPointers[1000];
	for(j=0; j<100: j++)
	{
		gettimeofday(&start, NULL);
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
	gettimeofday(%end, NULL);
	times[i] = (end.tv_sec*1000000+end.tv_usec)-(begin.tv_sec*1000000++begin.tv_usec);
	float average = average(times);
	return average;
	}
}

void grindB()
{
	struct timeval start, end;
	float times [100];
	int i, j;
    char * tmp = NULL;
    for (j=0; j<100, j++)
	{
		gettimeofday(&start, NULL);
		for(i = 0; j<1000; j++)
		{  
			tmp = (char*)malloc(1*sizeof(char));
       	 free(tmp);
		}
		gettimeofday(&end, NULL);

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
	char* myPointers[1000];
	int numFrees = 0;
	int numMallocs = 0;
	int numSaved = 0;
	int arrIndex = 0;
	int freeIndex = 0;
	int op, i;
	int spaceUsed = 0;
	int amtData = 0;
	initMyPointers(myPointers);

		while(numFrees<1000)
		{
			op = rand();
			amtData = 1+rand()%64;
			if (amtData+2+spaceUsed > 5000)
			{
				op = 1;
			}

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
				char* ptr = (char*)malloc(amtData*sizeof(char));
				if (ptr!=0)
				{	
					numMallocs ++;
					myPointers[arrIndex] = ptr;
					spaceUsed += amtData+2;
					arrIndex++;
					numSaved ++;
				}
			}
		}
	printf("arrIndex = %i, freeIndex = %i\n", arrIndex, freeIndex);
}

//start at mallocing 5000, then decrement by two, alternating malloc and free each time
void grindE()
{
	char * tmp = NULL;
	int k = 0;
	for(k = 0; k < 2497; k++)
	{
		tmp = (char*)malloc(4996-2*k);
		free(tmp);
	}
}

void grindF()
{
	char * p = NULL;
	int i = 0;
	for(i = 4900; i > 0; i-=100)
	{
		printf("F: %d\n",i);
		p = (char*)malloc(i*sizeof(char));
		free(p);
	}
}

float average(float [] arr)
{
	float i;
	float sum = 0;
	for (i=0; i<100; i++)
	{
		sum += arr[i];
	}
	return (sum/100);
}

void main(int argc, char * argv[])
{
int runtimes[6] = {0,0,0,0,0,0};

	float ATime = grindA();
	//i think %f prints a float but you should double check
	printf("Grind A average time: %f", ATime);
	//grindB();
	//grindC();
	//grindD();
	//grindE();
	//grindF();
//  int x;
//  free(&x);
//	char * sara = (char*)malloc(4998*sizeof(char));
	 
	printf("\n:)\n");	
//	free(sara);
}

