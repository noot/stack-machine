#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

struct Stack
{
    int top; // stack pointer
    unsigned capacity; // max size of stack. could use a linked list for dynamically sized stack, but chose a fixed size to emulate the EVM
    int* array; // the stack array 
};

unsigned program[] = { 0x0164, 0x01C8, 0x0301, 0x0200, 0x0000 };

/* stack */
struct Stack* stack;

/* program counter */
int pc = 0;
int imm = 0;

/* fetch the next word from the program */
int fetch()
{
  return program[ pc++ ];
}

/* instruction fields */
int instrNum = 0;

/* decode a word */
void decode( int instr )
{
  // first byte of instruction is the instruction number 
  instrNum = (instr & 0xFF00) >> 8;
  // second byte of instruction is immediate value
  imm = (instr & 0xFF);
}

/* the VM runs until this flag becomes 0 */
int running = 1;

/* evaluate the last decoded instruction */
void eval()
{
  switch( instrNum )
  {
    case 0:
      /* halt */
      printf( "halt\n" );
      running = 0;
      break;
    case 1:
      /* push */
      printf( "push #%d\n", imm);
      push(stack, imm);
      break;
    case 2:
      /* pop */
      printf( "pop %04X\n ", stack->array[ stack->top ] );
      pop(stack);
      break;
    case 3:
      /* add */
      printf( "add #%04X #%04X\n", stack->array[ stack->top ], stack->array[ stack->top - 1]  );
      int i = pop(stack);
      int j = pop(stack);
      int k = i + j;
      push(stack, k);
      break;
  }
}

/* display stack contents as 4-digit hexadecimal words */
void showStack()
{
  int i;
  printf( " stack : " );
  if(!isEmpty(stack)){
    for( i=(stack->top); i>=0; i-- )
      printf( "%04X ", stack->array[ i ] );
    }
    printf( "\n" );
}

void run()
{
  while( running )
  {
    showStack();
    int instr = fetch();
    decode( instr );
    eval();
  }
  showStack();
}

int main( int argc, const char * argv[] )
{
  stack = createStack(100);
  run();
  return 0;
}
