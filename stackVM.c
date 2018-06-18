#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

enum { MAX_PROGRAM_LENGTH = 256, MAX_OPCODE_LENGTH = 4 };

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
/* immediate */
int imm = 0;
/* link register */
int lr = 0;

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
  //instrNum = (instr & 0xFF00) >> 8;
  instrNum = instr;
  // second byte of instruction is immediate value
  // allows for immediate values up to 256
  //imm = (instr & 0xFF);
  //imm = fetch();
}

/* the VM runs until this flag becomes 0 */
int running = 1;

/* evaluate the last decoded instruction */
void eval()
{
  int imm;
  switch( instrNum )
  {
    case 0:
      /* halt */
      printf( "halt\n" );
      running = 0;
      break;
    case 1:
      /* push */
      imm = fetch();
      printf( "push #%d\n", imm );
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
      /* branch */
      lr = pc;
      i = pop(stack);
      printf( "b %04X\n", i );
      pc = i;
      break;
    case 17:
      /* branch if not zero */
      lr = pc;
      if(pop(stack) == 0) break;
      i = pop(stack);
      printf( "bnz %04X\n", i );
      pc = i;
      break;
    case 18: 
      /* branch if zero */
      lr = pc;
      if(pop(stack) != 0) break;
      i = pop(stack);
      printf( "bz %04X\n", i );
      pc = i;
      break;
    case 19:
      /* return */
      printf( "ret" );
      pc = lr;
      break;
    case 30:  
      /* swap */
      imm = fetch();
      printf( "swp %04X %04X\n", stack->array[stack->top], stack->array[stack->top - imm]);
      swap(stack, imm);
      break;
    case 31:
      /* cmp */
      printf( "cmp %04X %04X\n", stack->array[ stack->top ], stack->array[ stack->top - 1] );
      if(stack->array[ stack->top ] == stack->array[ stack->top - 1]) push(stack, 1);
      else push(stack, 0);
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
    unsigned temp;
    char op[MAX_OPCODE_LENGTH];

    // read file
    // operands are written in hex. change %x to %d for decimal
    while(fscanf(fp, "%s %x%*[^\n]", op, &temp) != EOF){
      // if comment, ignore
      if( strncmp(op, "//", 2) == 0 ) break;
      
      // print out program
      if ( strncmp(op, "push", strlen("push")) == 0 || strncmp(op, "swp", strlen("swp")) == 0 ) printf("%s %04x\n", op, temp);
      else printf("%s\n", op);

      // append instruction number into program array
      // strncmp returns 0 if strings are equal
      // push, swp use immediate value; other opcodes are standalone
      if ( strncmp(op, "halt", strlen("halt")) == 0 ) program[j] = 0;
      else if ( strncmp(op, "push", strlen("push")) == 0 ) {
          program[j] = 1;
          program[j+1] = temp;
          j++;
      }
      else if ( strncmp(op, "pop", strlen("pop")) == 0 ) program[j] = 2;
      else if ( strncmp(op, "add", strlen("add")) == 0 ) program[j] = 3;
      else if ( strncmp(op, "sub", strlen("sub")) == 0 ) program[j] = 4;
      else if ( strncmp(op, "mod", strlen("sub")) == 0 ) program[j] = 5;
      else if ( strncmp(op, "b", strlen("b")) == 0 ) program[j] = 16;
      else if ( strncmp(op, "bnz", strlen("bnz")) == 0 ) program[j] = 17;
      else if ( strncmp(op, "bz", strlen("bz")) == 0 ) program[j] = 18;
      else if ( strncmp(op, "ret", strlen("ret")) == 0 ) program[j] = 19;
      else if ( strncmp(op, "swp", strlen("swp")) == 0 ) {
          program[j] = 30;
          program[j+1] = temp;
          j++;
      }
      else if ( strncmp(op, "cmp", strlen("cmp")) == 0 ) program[j] = 31;
      else if ( strncmp(op, "nop", strlen("nop")) == 0 ) program[j] = 255;
      else j--;
      
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
