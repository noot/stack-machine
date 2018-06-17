#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

enum { MAX_PROGRAM_LENGTH = 256 };

struct Stack
{
    int top; // stack pointer
    unsigned capacity; // max size of stack. could use a linked list for dynamically sized stack, but chose a fixed size to emulate the EVM
    int* array; // the stack array 
};

unsigned program[MAX_PROGRAM_LENGTH];

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
  // allows for up to 256 opcodes
  instrNum = (instr & 0xFF00) >> 8;
  // second byte of instruction is immediate value
  // allows for immediate values up to 256
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
      printf( "add %04X %04X\n", stack->array[ stack->top ], stack->array[ stack->top - 1]  );
      int i = pop(stack);
      int j = pop(stack);
      int k = i + j;
      push(stack, k);
      break;
    case 4:
      /* sub */
      printf( "sub %04X %04X\n", stack->array[ stack->top ], stack->array[ stack->top - 1]  );
      i = pop(stack);
      j = pop(stack);
      k = i - j;
      push(stack, k);
      break;
    case 5:
      /* mod */
      printf( "mod %04X %04X\n", stack->array[ stack->top ], stack->array[ stack->top - 1]  );
      i = pop(stack);
      j = pop(stack);
      k = i % j;
      push(stack, k);
      break;
    case 16:
      /* jump */
      printf( "jump %04X\n", imm );
      pc = imm;
      break;
    case 17:
      /* jumpeq */
      printf( "jumpeq %04X\n", imm );
      i = pop(stack);
      j = pop(stack);
      if(i == j) pc = imm;
      push(stack, j);
      push(stack, i);
      break;
    default:
      /* nop */
      printf( "nop\n" );
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
  else {
    printf( "empty" );
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
  int i = 1;
  printf("programs to run: \n");
  for(i; i < argc; i++) printf("%d: %s\n", i, argv[i]);
  //printf("%d\n", argc);

  stack = createStack(100);

  for(i = 1; i < argc; i++) {
    /* open and check file */
    const char *arg = argv[i];
    FILE *fp = fopen(arg, "r");
    if(fp == NULL) {
      printf( "could not open %s\n", arg);
      break;
      //return 1;
    }
  
    /* read lines in file and load into memory */
    printf("program to next execute: %s\n", arg);
    int j = 0;
    while(fscanf(fp, "%x%*[^\n]", &program[j]) != EOF) {
      //printf("%04x\n", program[j]);
      j++;
      if(j > MAX_PROGRAM_LENGTH) break;
    }
    fclose(fp);

    /* starting execution, reset sp, pc and running flag */
    printf("starting execution of %s: \n", arg);
    stack->top = -1;
    pc = 0;
    running = 1;
    run();
  }

  return 0;
}
