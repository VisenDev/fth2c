/* Forth Primitives */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define cap 16

typedef uintptr_t Cell;
Cell stack[cap] = {0};
Cell stack_len = 0;
Cell return_stack[cap] = {0};
Cell * latest_variable = NULL;

void push(Cell value) {
    assert(stack_len < cap);
    stack[stack_len] = value;
    stack_len += 1;
}

Cell pop(void) {
    assert(stack_len > 0);
    assert(stack_len < cap);
    /* 0 1 2 3  */
    /* len -> 4 */
    stack_len -= 1;
    return stack[stack_len];
}


/* primitives */
void fth_add(void) {
    assert(stack_len >= 2); 
    push(pop() + pop());
}
void fth_sub(void) {
    assert(stack_len >= 2); 
    push(pop() - pop());
}
void fth_mul(void) {
    assert(stack_len >= 2); 
    push(pop() * pop());
}
void fth_div(void) {
    assert(stack_len >= 2); 
    push(pop() / pop());
}
void fth_mod(void) {
    assert(stack_len >= 2); 
    push(pop() % pop());
}
void fth_lit(Cell value) {
    push(value);
}
void fth_emit(void) {
    putchar(pop());
}
void fth_dup(void) {
    Cell value = pop();
    push(value);
    push(value);
}
void fth_swap(void) {
    Cell top = pop();
    Cell next = pop();
    push(top);
    push(next);
}
void fth_print_top(void) {
    Cell top = pop();
    printf("%lu ", top);
}
void fth_eql(void) {
    Cell top = pop();
    Cell next = pop();
    push(top == next);
}
void fth_less_than(void) {
    Cell top = pop();
    Cell next = pop();
    push(next < top);
}
void fth_greater_than(void) {
    Cell top = pop();
    Cell next = pop();
    push(next > top);
}
void fth_greater_than_eql(void) {
    Cell top = pop();
    Cell next = pop();
    push(next >= top);
}
void fth_less_than_eql(void) {
    Cell top = pop();
    Cell next = pop();
    push(next <= top);
}
void fth_and(void) {
    Cell top = pop();
    Cell next = pop();
    push(top && next);
}
void fth_or(void) {
    Cell top = pop();
    Cell next = pop();
    push(top || next);
}
