#include "mymalloc.h"
#include <stdlib.h>
#include <stdio.h>
//how is input happening? assuming preprocessing willtake care of it, i.e. don't need main method

int main(int argc, char** argv)
{
}
int i;
//create the malloc-able array; cast as short* and initialize all empty spaces to 0
*(short*) myBlock = 4998;
for (i=1; i<2500; i++)
{
	*(short*) (myBlock +i) = 0;
}






char* myMalloc (int numRequested)
{
	if(numRequested +2 <= *(short*) (myBlock[0]))
		{
			int index = checkContiguous(myBlock, numRequested+2)
			{
				if (index != -1)
				{
					return index;
				}
				else
				{
					return defrag(myBlock);
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

//or is it a short*?
/*char* defrag (char* myBlock)
{
	short* prev = (short*)(myBlock);
	short* curr = (short*)(myBlock);
	int count = 0;
	while (count <5000)
	{
		*curr = *(short*)(myBlock + count)
		{
			if (*curr%2 ==1)
			{
				curr += *curr;
				count 
				prev = curr;
			}

		}
	}
}*/

void free (char* p)
{
	
}












