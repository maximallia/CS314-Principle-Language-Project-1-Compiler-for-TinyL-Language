/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Spring 2014                              *
 *  Authors: Ulrich Kremer                   *
 *           Hans Christian Woithe           *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"


//Instruction* head;

void deadlift(Instruction* head){ //identify critical pt with recurs.
  int instrField2 = 0;//for and, or, add, sub, mul, to locate the field2&3
  int instrField3 = 0;
  
  Instruction* ptr = head;
  Instruction* pre = ptr->prev;

  switch(ptr->opcode){//from current node's opcode,check prev nodes

  case STORE:
    while( pre != NULL){//storing,thus prev affects;use recurs.
      if( pre->opcode!=STORE && pre->opcode!=READ && pre->opcode!=WRITE ){//if pre->field1 is var
	if( (pre->field1 == ptr->field2) ){//if pre->field1 is reg. 
	  pre->critical = 'y';
	  //printf("STORE\n");
	  deadlift(pre);
	  break;
	}
      }
      else if(pre->opcode==LOADI)
      pre = pre->prev;
    }
    return;

  case LOAD:
    while(pre != NULL){//need to search reg
      if( (pre->opcode == STORE) || (pre->opcode==READ) ){
        if( (pre->field1 == ptr->field2) ){
          pre->critical = 'y';
          //printf("LOADED1\n");
          //instrField2 = 1;
          deadlift(pre);
        }
      }
      if(pre->opcode == LOAD && pre->field1==ptr->field1){
	pre->critical='n';
      }
      pre = pre->prev;
    }
    return;
    
  case LOADI://constant,no reg needed
    while(pre!=NULL){
      if(pre->opcode==LOADI && pre->field1==ptr->field1){
	pre->critical='n';//if same var
      }
      pre = pre->prev;
    }
    ptr->critical = 'y';
    return;

  case ADD:
  case SUB:
  case MUL:
  case OR:
  case AND:
    while(pre != NULL){
      //ADD, SUB, MUL, OR, AND; compare pre, thus recursion
      if(pre->opcode!=STORE && pre->opcode!=WRITE && pre->opcode!= READ){
	if( (pre->field1 == ptr->field2)&& (instrField2==0)){
	  pre->critical = 'y';
	  //printf("ADDED1\n");
	  instrField2 = 1;
	  deadlift(pre);
	}
	if( (pre->field1 == ptr->field3) && (instrField3 == 0) ){
	  pre->critical = 'y';
	  //printf("ADDED2\n");
	  instrField3 = 1;
	  deadlift(pre);
	}
	if(instrField2 == 1 && instrField3 == 1){//both field2,3 filled
	  //printf("ADDED3\n");
	  break;
	}
      }
      pre = pre->prev;
    }
    return;

  case WRITE://print; move on with recurs.
    while( pre != NULL){
      if(  (pre->field1 == ptr->field1) && (pre->opcode == STORE||pre->opcode==READ)){
	pre->critical = 'y';
	//printf("WRITTEN\n");
	deadlift(pre);
	break;
      }
      pre = pre->prev;
    }
    return;
    
  case READ://only reading, no effect on instr., thus use ptr
    ptr->critical = 'y';
    //printf("READ\n");
    return;
  }

}

void runner(Instruction* head){//runs everything

  Instruction* ptr = head;//infinite runon, FIXED!!
  
  while(ptr != NULL){//run first time, fill in critical for WRITE and READ
    //printf("aaaaaPTR\n");                                                                                                                                                       
    if( (ptr->opcode == WRITE) || (ptr->opcode == READ)){
      ptr->critical = 'y';
    }
    ptr = ptr->next;
  }

  Instruction* ptr2 = head;//used to check for dead codes for all pre before ptr2
  while( ptr2 != NULL){
    //printf("aaaaaPTR2\n");
    if( (ptr2->critical == 'y') && (ptr2->opcode == WRITE) ){//no READ for no effect                                                                                                
      deadlift(ptr2);//check all pre                                                                                                                                                
    }
    ptr2 = ptr2->next;
  }
  
  Instruction* fptr = head;
  while(fptr != NULL){
    if(fptr->critical != 'y'){//dead code, skip
      //printf("aaaaFPTR\n");
      Instruction* temp = fptr->next;
      if(fptr->prev != NULL){
        fptr->prev->next = fptr->next;//skip fptr, connect prev with next
      }
      if(fptr->next != NULL){// skip fptr, connect next with prev
        fptr->next->prev = fptr->prev;
      }
      free(fptr);//free fptr
      fptr = temp;
    }
    else{
      fptr = fptr->next;
    }
  }

}


int main()
{
  Instruction* head;
  
  head = ReadInstructionList(stdin);

  if (!head) {
    WARNING("No instructions\n");
    exit(EXIT_FAILURE);
  }
  // YOUR CODE GOES HERE 

  runner(head);

  if (head) {
    PrintInstructionList(stdout, head);
    DestroyInstructionList(head);
  }
  return EXIT_SUCCESS;
}

