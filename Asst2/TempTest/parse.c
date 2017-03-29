#include "tokenizer.h"
recordNode* tokenize(FILE* file, char* fileName)
{
    recordNode* head = NULL;
    char buffer [5000];
    int index = 0;
	recordNode* curr = NULL;
	char c = fgetc(file);
	while (c != EOF)
    {
		index = 0;
		//skip all non-alphanumeric garbage THIS NEEDS TO SKIP ON FIRST NUMBERS TOO APPARENTLY
		while (c!=EOF && !isalpha(c))
		{
			c = fgetc(file);
		}
		//get all alphanumerics in current token
		while(c!= EOF && isalnum(c))
		{
			buffer[index] = c; //does this move the pointer every time?
			index ++;
			c = fgetc(file);
		}
		//make sure string is null terminated
		buffer[index] = '\0';
		char* currTok = malloc(sizeof(char)*index);
		if(strlen(buffer)>0)
        {
		//+1?
			memcpy(currTok, buffer, strlen(buffer));
			if(head == NULL)
       	 	{
            	head = makeNode(fileName, currTok);
            	curr = head;
        	}
        	else
        	{
            	curr->next = makeNode(fileName, currTok);
            	curr = curr-> next;
        	}
    	}
	}
	myToLower(head);
    return head;
}


void addToTable(recordNode* list, hashTable* hTable , char* fileName)
{
	
	int count =0;
    //slot in the hashTable according to leading letter
    int index;
    //leading letter
    char leading;
    //hashTable* hTable = makeHashTable(36);
	while(list!=NULL)
	{
		count ++;
		leading = list->token[0];
        	index = leading;
		//alphas first in table, numerics second
		if (!isalpha(leading))
			{
				index += 26;
			}
		else
			{
				index -=97;
			}
		//node to be inserted
        recordNode* node = makeNode(fileName, list->token);
        //if node is to be inserted at front of list
		if (hTable->table[index] == NULL || sortalnum(hTable->table[index]->token, node->token)<0)    
			{
                node->next = hTable->table[index];
                hTable->table[index] = node;
			}
        //if node is second node or later
            else
            {
                recordNode* curr = hTable->table[index];
                recordNode* prev = curr;
            //while string to be inserted comes after existing strings
                while(curr!=NULL && sortalnum(curr->token, node->token)>0)
                {
                    prev = curr;
                    curr = curr->next;
                }
			if (curr!=NULL && sortalnum(curr->token, node->token)==0)
			{
				if (strcmp(curr->fileName, node->fileName)!=0)
				{
								//HERE BEGINS THE NEW TERRITORY
							if(curr->next != NULL && strcmp(curr->next->token, node->token)==0 && strcmp(node->fileName, curr->next->fileName)==0){
								//printf("well it might be working");	
								curr->next->count++;
						//free(node);
							}else{
							//printf("original file %s, current file  %s, token = %s\n", curr->fileName, node->fileName, node->token);
							node->next = curr->next;
							curr -> next = node;
							//printTable(hTable);
							}
						}
						else
						{
							curr -> count ++;
							//free(node);
						}
					}
					else
						{
						node->next = curr;
						prev->next = node;
						}
				}
				list = list->next;		
			}
			//destroyList(list);
    return;
}







