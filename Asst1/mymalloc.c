#include "mymalloc.h"
#include <stdlib.h>
#include <stdio.h>
//how is input happening? assuming preprocessing willtake care of it, i.e. don't need main method
//ALL FREE SPACE SHOULD INDICATE HOW MUCH COMES AFTER IT
//how to deal with used space? can't fill it with any data because that's the user's space to utilize...?

int main(int argc, char** argv)
{
}
int i;
int freeSpace = 4998;
//create the malloc-able array; cast as short* and initialize all empty spaces to 0
for (i=0; i<5000; i++)
{
	*(short*)(myBlock+i) = freeSpace;
	i+=2;
	freeSpace -= 2;
}



char* myMalloc (int numRequested)
{
	if(numRequested +2 <= *(short*)myBlock)
		{
			int index = checkContiguous(myBlock, numRequested+2)
			{
				if (index != -1)
				{
					*(short*)myBlock -= numRequested+2;
					return index;
				}
				else
				{
					return (char*)defrag(myBlock);
				}
			}
		}
	else
		return enum boolean answer = FALSE;
}

//numRequested = actual # of bytes requested, plus 2 for metadata
//return type???
short* checkContiguous (char* myBlock, int numRequested)
{
	short i=2;
	//keep running count of amount of contiguous memory available
	int contigCount;
	//indicates number of bytes following current slot
	short numAdd;

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
	return -1;
}

//find contiguous blocks of free space and combine them to a single large block
char* defrag (char* myBlock)
{
	short* prev = (short*)(myBlock);
	short* curr = (short*)(myBlock);
	//# of bytes so far accounted for - begins at 2 because of the array metadata always present at 0
	int count = 2;

	while (count <5000)
	{
		prev = myBlock+count;
		//if the current block of memory is free, check the next block to see if can combine
		if (*(short*)(myBlock+count) %2 == 0)
		{
			//add the amount indicated, plus two for the block itself
			count += 2+*(short*)(myBlock+count);
			curr = (short*)(myBlock+count);
			if (*curr%2==0)
			{
				*prev += *curr +2;
			}
		}
	}
}

void free (char* p)
{
	//make sure this pointer was within the bounds of the array
	if (p<myBlock+2 || p>myBlock+4996)
		return -1;
	
	//error handling in case the location wasn't actually malloced, or if it points to an area that isn't metadata
	if (*(short*)place %2 !=1 || *p == '_' || *p == '*' )
		return -1;

	//decrement the number there to be even, indicating that x many bytes after it are free
	*(short*)place -=1;
}












