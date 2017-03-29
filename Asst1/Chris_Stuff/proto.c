
boolean checkSpace(char* myBlock, size_t numReq)

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

char * findSpace(char* myBlock, unsigned short numReq)
{

	//tracks how far weve gone, loop terminates after 5000 bytes
        unsigned short distanceTrav = 0;
	//keeps trace of the current value in the metadata that we are currently looking at in the loop
        unsigned short currMeta = 0;
        
        while(distanceTrav<=5000){
        	//takes the value of currMeta for this iteration of loop
        	currMeta = *(unsigned short*)myBlock;
        	//if you find a portion of memory which is free and sufficiently large then success
        	if((currMeta%2==0) && (currMeta>=numReq)){
			//can return a ptr to the user data located after this metadata segment
			myBlock += 2;				
			return (char*) myBlock;
		//else need to jump to next metadata
		}else{
		//this calculation catches both free and used jumps (because of mod artih)
			unsigned short increment = currMeta - (currMeta%2) + 2;
		//increment block to new position (CHECK to make sure this only affects myBlock inside this method
			myBlock += increment;
			//and increment distanceTraveled 
			distanceTrav += increment;
		}
	}
	return NULL;
}
