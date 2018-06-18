#ifndef STACK_H_   /* Include guard */
#define STACK_H_

struct Stack;
struct Stack* createStack(unsigned capacity);
int isFull(struct Stack* stack);
int isEmpty(struct Stack* stack);
void push(struct Stack* stack, int item);
int pop(struct Stack* stack);
void swap(struct Stack* stack, int i);

#endif // STACK_H_
