#include "mymalloc.h"
#include <stdlib.h>
#include <stdio.h>

boolean isInitialized = FALSE;

void* mymalloc (size_t numRequested, char* file, int line)
{
	if (!isInitialized)
		{
			initArray(myBlock);
		}
	//ensure that the input is even and not less than 1 or over 4998
	numRequested = validateInput(numRequested);
	char* thatSoMeta;
	//might want to include error message here
	if (numRequested == 0)
		return 0;
	if (checkSpace(myBlock, numRequested))
	{
		thatSoMeta = findSpace(myBlock, numRequested);
	}
	else
	{
		defrag(myBlock);
		printf("defragged\n");
		thatSoMeta = findSpace(myBlock, numRequested);
	}
	if(thatSoMeta == NULL)
	{
		printf("INSUFFICIENT AVAILABLE MEMORY - ALLOC DENIED \n");
		return 0;
	}
	
	void* test = mallocDetails(numRequested, thatSoMeta);
	//printf("num allocated: %hu \n", *(short*)(test));
	return test + 2;//mallocDetails(numRequested, thatSoMeta);	
}
//find whether there is adequate free space in block 
boolean checkSpace(char* myBlock, size_t numReq)
{
		//tracks how far we've gone, loops terminates after 5000 bytes
        unsigned short consumed = 0;
        //keeps trace of the current value in the metadata that we are currently looking at in the loop
        unsigned short currMeta = 0;

        while(consumed<5000)
		{
                //takes the value of currMeta for this iteration of loop
                currMeta = *(unsigned short*)myBlock;
				//if you find a portion of memory which is free and sufficiently large then success
                if((currMeta%2)==0 && (currMeta>=numReq))
				{
                    return TRUE;
                }
				else
				{
        			//this calculation catches both free and used jumps (because of mod arith)
                    
//					printf("currMeta: %i, numReq: %i \n", currMeta, numReq);
					unsigned short increment = currMeta - (currMeta%2) + 2;
        			//increment block to new position (CHECK to make sure this only affects myBlock inside this method
                    myBlock += increment*sizeof(char);
      				//and increment distanceTraveled 
                    consumed += increment*sizeof(char);
                }
        }
		return FALSE;
}
//returns pointer to first incidence of sufficiently large block
char* findSpace(char* myBlock, unsigned short numReq)
{
	//tracks how far down the array has been traveled
	unsigned short consumed = 0;
	//keeps trace of value contained in current metadata block
	unsigned short currMeta = 0;
	
	while(consumed<5000)
	{
		currMeta = *(unsigned short*)myBlock;
		//return pointer to start of META (not user!) data block if sufficient size free block is found
		//TOOK AWAY THE PLUS TWO HERE
		if((currMeta%2==0) && (currMeta>=numReq))
		{
//			printf("head block: %hu\n", currMeta);
			return myBlock;
		}
		else
		{
			//catches both free and used jumps through mod arith
			unsigned short increment = currMeta - (currMeta%2) + 2;
//			printf("currMeta: %hu \t increment: %hu \n", currMeta, increment);
			myBlock += increment*sizeof(char);
			//increment distanace traveled
			consumed += increment;
			//printf("consumed: %hu \n", consumed);
		}
	}
	return NULL;
} 

//find contiguous blocks of free space and combine them to a single large block
void defrag (char* myBlock)
{
	unsigned short consumed = 0;
	unsigned short* home = (unsigned short*) myBlock;
	unsigned short* probe = (unsigned short*) myBlock;
	unsigned short toAdd = 0;
	while(consumed < 5000)
	{
		while (consumed <5000 && *home%2 == 1)
		{
			printf("odd\n");
			consumed += 2+*home;
			home += (1+*home)/2;
			probe += (1+*probe)/2;
		}

		//printf("probe: %hu\n", *probe);
		//printf("adding %hu\n", (2+*probe)/2);
		probe += (2+*probe)/2;
		//printf("probe: %hu\n", *probe);
		while(consumed<5000&&(*probe)%2==0)
		{
				//printf("probe: %hu\n", *probe);
				*home += 2+*probe;
				consumed += 2+*probe;
				probe += (2+*probe)/2;
		}
		//printf("consumed: %hu, home: %hu\n", consumed, *home);
		if (*(probe)%2 == 1)
		{
			home = probe;
		}

	}
}

//return boolean true for success and failure
boolean myfree(void* target, char* file, int line)
{
	if (!isInitialized)
		{
			initArray(myBlock);
			isInitialized = TRUE;
		}
//	printf("in target: %p \n", (void*)target);
//	printf ("target: %hu \n", *(unsigned short*)(target-2));
	void* targetFree = target - 2;
	void* ptr = (void*)myBlock;
	unsigned short distance = 0;
	while (targetFree != ptr && distance < 5000)
	{
		distance += 2+ *(unsigned short*)ptr - (*(unsigned short*)ptr) %2;
//		printf("DISTANCE: %hu \n", distance);
		ptr += *(unsigned short*)ptr +2 - (*(unsigned short*)ptr)%2;
	}
	//here either targetFree = ptr or distance > 5000
	if (targetFree == ptr)
	{
	//	printf("%hu \n", *(unsigned short*)ptr);
		if ((*(unsigned short*)ptr) %2 == 1)
		{
//			printf("merp\n");
			*(unsigned short*)ptr -= 1;
			return TRUE;
		}
	}
	printf("ERROR: INVALID ADDRESS, CANNOT FREE\n");
	return FALSE;
}

//set initial amount of free space and zero out the array in case of garbage null terminators
void initArray(char* myBlock)
{
	int i;
	for (i=0; i<5000; i++)
	{
		myBlock[i] = '0';
	}
	*(unsigned short*)myBlock = (unsigned short)4998;
	isInitialized = TRUE;
}

size_t validateInput(size_t numRequested)
{
	//must be within array bounds
	if (numRequested<=0 || numRequested>4998)
		{
			printf("INVALID REQUEST, CANNOT ALLOC\n");
			return 0;
		}
	//allocation must be even
	return (numRequested + numRequested%2);
}

void* mallocDetails(size_t numReq, char* index)
{
	unsigned short total = *(unsigned short*)index;
	if (total > numReq)
	{
		unsigned short* leftovers = (unsigned short*) (index+2+numReq*sizeof(char));
		*leftovers = total - (numReq+2);
	}

	*(unsigned short*)index = numReq+1;
	//printf("remaining free space: %hu \n", *(unsigned short*)(index+2+numReq*(sizeof(char))));
	return (void*)index;
}
