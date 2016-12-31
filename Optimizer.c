/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Fall 2016                              *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"
#include <ctype.h>

 

int * appendToArray(int item, int * array, int length){
	array = (int*)realloc(array, (sizeof(int) * length) + (sizeof(int)));
	length++;
	array[length - 1] = item;
	return array;
}

int * deleteFromArray(int * array, int index, int length){

	int i;
	for (i = index + 1; i < length; i++){
		array[i - 1] = array[i];
	}
	length--;
	array = (int*)realloc(array, sizeof(int) * length);
	return array;
}

void optimize(Instruction *head){
	
	Instruction *tail = LastInstruction(head);
	Instruction *instr = tail;
	int * contributingRegisters = (int*)malloc(sizeof(int));
	int * contributingVariables = (int*)malloc(sizeof(int));
	int reglength = 0;
	int varlength = 0;

	//locate write operation
	while (instr != NULL && instr->opcode != WRITE){
		//printf("Stuck in loop\n");
		instr = instr->prev;
	}
	//printf("WRITE FOUND\n");
	contributingVariables = appendToArray(instr->field1, contributingVariables, varlength);
	varlength++;
	
	instr = instr->prev;
	int receivingRegister = 0;
	while (instr != NULL){
		Instruction *prev = instr->prev;
		if (instr->opcode == ADD  || instr->opcode == MUL || instr->opcode == SUB){
			//printf("ADD, MUL, SUB\n");
			if (instr->field1 == receivingRegister){
				//printf("appending two registers\n");
				contributingRegisters = appendToArray(instr->field2, contributingRegisters, reglength);
				reglength++;
				contributingRegisters = appendToArray(instr->field3, contributingRegisters, reglength);
				reglength++;
				receivingRegister = instr->field1;
			}
			else {
				int i;
				for(i = 0; i < reglength; i++){
					if (instr->field1 == contributingRegisters[i]){
						//printf("appending two registers\n");
						contributingRegisters = appendToArray(instr->field2, contributingRegisters, reglength);
						reglength++;
						contributingRegisters = appendToArray(instr->field3, contributingRegisters, reglength);
						reglength++;
						receivingRegister = instr->field1;
						break;
					}	
				}
				if (i == reglength){
					//printf("instruction deleted\n");
					prev->next = instr->next;
					free(instr);
					instr = NULL; 
				}
			}
			
		}
		else if (instr->opcode == LOAD || instr->opcode == LOADI){
			//printf("LOAD, LOAD1\n");
			
			int i;
			for(i = 0; i < reglength; i++){
				if (instr->field1 == contributingRegisters[i]){
					if (isalpha(instr->field2)){
						contributingVariables = appendToArray(instr->field2, contributingVariables, varlength);
						varlength++;
						
					}
					break;
				}	
			}
			if (i == reglength){
				//printf("instruction deleted\n");
				prev->next = instr->next;
				free(instr);
				instr = NULL;	
			}
		}
		else if (instr->opcode == STORE){
			//printf("STORE\n");
			int i;
			for(i = 0; i < varlength; i++){
				if (instr->field1 == contributingVariables[i]){
					//printf("appending one register\n");
					contributingRegisters = appendToArray(instr->field2, contributingRegisters, reglength);
					reglength++;
					receivingRegister = instr->field2;
					break;	
				}
			}
			if (i == varlength){
				//printf("instruction deleted\n");
				prev->next = instr->next;
				free(instr);
				instr = NULL;	
			}
		}
		else if(instr->opcode == WRITE){
			//printf("WRITE\n");
			contributingVariables = appendToArray(instr->field1, contributingVariables, varlength);
			varlength++;
		}
		//printf("reglength = %d\n", reglength);
		//printf("varlength = %d\n", varlength);
		instr = prev;
	}
	free(contributingRegisters);
	free(contributingVariables);
}

int main()
{
	Instruction *head;

	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}

	optimize(head);
	if (head) {
		PrintInstructionList(stdout, head);
		DestroyInstructionList(head);
	}
	return EXIT_SUCCESS;
}

