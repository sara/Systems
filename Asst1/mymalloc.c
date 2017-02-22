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

//numRequested = actual # of bytes requested, plus 2 for metadata
unsigned short* checkContiguous (char* myBlock, size_t numRequested)
{
	unsigned short i=2;
	//keep running count of amount of contiguous memory available
	int contigCount;
	//indicates number of bytes following current slot
	unsigned short numAdd;

	while (i<5000)
	{
		//safely add the value of i to the string address
		myBlock += i*sizeof(char);
		//number at current index;
		numAdd = *(short*)myBlock;

		//skip ahead by the number of blocks indicated if the stretch of memory is full (odd numbers indicate full, even implies empty)
		if (numAdd % 2 == 1)
		{
			//add 1 because the unsigned short in the current slot takes up 2 bytes, but the number indicated is one larger than the actual space taken (to indicate used)
			i += numAdd+1;
			contigCount = 0;
		}
		else

		{
			i+= numAdd+2;
			contigCount += numAdd+2;
			if (contigCount == numRequested)
			{
				return (short*)(myBlock-contigCount);
			}
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

//set initial metadata block, and indicate free space at all available blocks in empty array
void initArray(char* myBlock)
{
	int i;
	if (!isInitialized)
		{
			*(unsigned short*)myBlock = (unsigned short) 4998;
			for (i=2; i<5000; i+=2)
			{
				*(short*)(myBlock+i) = 4998-i;
			}
		}
}
//for some reason the length comes out as 84 and 4?!?!?
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
