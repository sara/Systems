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
	int i=1;
	//keep running count of amount of contiguous memory available
	int contigCount;

	while (i<5000)
	{
		if (*(short*)(myBlock+i) % 2 == 1)
		{
			i += myBlock[i];
			contigCount = 0;
		}
		else
		{
			contigCount ++;
			if (contigCount == numRequested)
			{
				return (short*)(myBlock+i);
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












