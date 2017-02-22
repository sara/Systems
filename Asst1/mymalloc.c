#include "mymalloc.h"
#include <stdlib.h>
#include <stdio.h>

boolean isInitialized = FALSE;
//how is input happening? assuming preprocessing will take care of it, i.e. don't need main method
//ALL FREE SPACE SHOULD INDICATE HOW MUCH COMES AFTER IT
//HOW CAN I FILL THE ARRAY METADATA SLOT!?

void* mymalloc (size_t numRequested)
{
	initArray(myBlock);
	printArr(myBlock);	
	if (numRequested <= 0)
		{
			printf("Invalid size request\n");
			return NULL;
		}
	//only even blocks of memory can be allocated
	if (numRequested % 2 == 1)
		numRequested ++;

	//check that there's enough space for both user data and metadata	
	if(numRequested +2 <= *(unsigned short*)myBlock)
		{
			//get index to place metadata
			unsigned short* index = checkContiguous(myBlock, numRequested+2);
		{
				if (index == NULL )
				{
					defrag(myBlock);
					*index = *checkContiguous(myBlock, numRequested + 2);
					if (index == NULL)
						{
							return NULL;
						}
				}
				
				else
				{
					//decrement the amount requested+metadata from the master header
					*(unsigned short*)myBlock -= (unsigned short) numRequested+2;
					//update the amount of space remaining after the block being used
					*(unsigned short*)(myBlock + *index + numRequested + 2 ) = *(unsigned short*)(myBlock+*index) - (unsigned short)(numRequested+2);
					//indicate that the current block is full
					*(unsigned short*)(myBlock+*index+numRequested) = (unsigned short)(numRequested + 1);
					return (void*)index;
				}
			}
		}
}
//find whether there is adequate free space in block 
boolean checkSpace(char* myBlock, size_t numReq)
{
        //tracks how far weve' gone, loops terminates after 5000 bytes
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
                    unsigned short increment = currMeta - (currMeta%2) + 2;
        			//increment block to new position (CHECK to make sure this only affects myBlock inside this method
                    myBlock += increment*sizeof(char);
      				//and increment distanceTraveled 
                    consumed += increment*sizeof(char);;
                }
        }
		return FALSE;
}
//returns pointer to first incidence of sufficiently large block
char* findSpace(char* myBlock, unsigned shot numReq)
{	
	//tracks how far down the array has been traveled
	unsigned short consumed = 0;
	//keeps trace of value contained in current metadata block
	unsigned short currMeta = 0;
	
	while(consumed<5000)
	{
		currMeta = *(unsigned short*)myBlock;
		//return pointer to start of META (not user!) data block if sufficient size free block is found
		if((currMeta%2==0) && (currMeta>=numReq))
		{
			myBlock += 2;
			return myBlock;
		}
		else
		{
			//catches both free and used jumps through mod arith
			unsigned short increment = currMeta - (currMeta%2) + 2;
			myBlock += increment*sizeof(char);
			//increment distanace traveled
			consumed += increment;
		}
	}
	return NULL;
} 

//find contiguous blocks of free space and combine them to a single large block
void defrag (char* myBlock)
{
	unsigned short* prev = (unsigned short*)(myBlock);
	unsigned short* curr = (unsigned short*)(myBlock);
	//# of bytes so far accounted for - begins at 2 because of the array metadata always present at 0
	int count = 2;

	while (count <5000)
	{
		prev = (unsigned short*)myBlock+count;
		//if the current block of memory is free, check the next block to see if can combine
		if (*(unsigned short*)(myBlock+count) %2 == 0)
		{
			//add the amount indicated, plus two for the block itself
			count += 2+*(unsigned short*)(myBlock+count);
			curr = (unsigned short*)(myBlock+count);
			if (*curr%2==0)
			{
				*prev += *curr +2;
			}
		}
	}
}

int myfree (char* p)
{
	int answer;
	//make sure this pointer was within the bounds of the array
	if (p<myBlock+2 || p>myBlock+4996)
		answer = -1;
	
	//error handling in case the location wasn't actually malloced, or if it points to an area that isn't metadata
	else if (*(unsigned short*)p %2 !=1 || *p == '_' || *p == '*' )
		answer = -1;

	//decrement the number there to be even, indicating that x many bytes after it are free
	else *(unsigned short*)p -=1;
	 	answer = 0;

	return answer;
	
}

//set initial amount of free space and zero out the array in case of garbage null terminators
void initArray(char* myBlock)
void printArr(char* myBlock)
{
	int i;
	printf("%i", sizeof(myBlock)/sizeof(char));
	printf("%i\n\n", sizeof((unsigned short*)(myBlock))/sizeof(unsigned short));
	for (i=0; i<sizeof(myBlock)/sizeof(unsigned short); i++)
	{
		printf("%hu\n", *(unsigned short*)(myBlock + i*sizeof(unsigned short)));
	}
}




