// C program for array implementation of stack
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "stack.h"
 
// A structure to represent a stack
struct Stack
{
    int top; // stack pointer
    unsigned capacity; // max size of stack. could use a linked list for dynamicly sized stack, but chose a fixed size to emulate the EVM
    int* array; // the stack array 
};
 
// function to create a stack of given capacity. It initializes size of
// stack as 0
struct Stack* createStack(unsigned capacity)
{
    struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (int*) malloc(stack->capacity * sizeof(int));
    return stack;
}
 
// Stack is full when top is equal to the last index
int isFull(struct Stack* stack)
{   return stack->top == stack->capacity - 1; }
 
// Stack is empty when top is equal to -1
int isEmpty(struct Stack* stack)
{   return stack->top == -1;  }
 
// Function to add an item to stack.  It increases top by 1
void push(struct Stack* stack, int item)
{
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
    //printf("%d pushed to stack\n", item);
}
 
// Function to remove an item from stack.  It decreases top by 1
int pop(struct Stack* stack)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top--];
}

// peek at top item on stack
int peek(struct Stack* stack) 
{
    if(isEmpty(stack)) return INT_MIN;
    return stack->array[stack->top];
}

// swap top item on stack with ith item below the top
void swap(struct Stack* stack, int i)
{
    if(stack->top < i) return;
    struct Stack* temp = createStack(i);
    int topVal = pop(stack);
    for(int j = 1; j < i; j++) push(temp, pop(stack));
    int swapVal = pop(stack);
    push(stack, topVal);
    for(int j = 0; j < i; j++) push(stack, pop(temp));
    push(stack, swapVal);
}
