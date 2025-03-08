/* Forth Primitives */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define cap 16

typedef long Cell;
Cell stack[cap] = {0};
Cell stack_len = 0;
Cell return_stack[cap] = {0};
Cell return_stack_len = 0;
Cell tmp = 0;

#define heap_cap 100000
Cell heap[heap_cap] = {0};
Cell heap_len = 0;

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
    Cell top = pop();
    Cell next = pop();
    push(next - top);
}
void fth_mul(void) {
    push(pop() * pop());
}
void fth_div(void) {
    Cell top = pop();
    Cell next = pop();
    push(next / top);
}
void fth_mod(void) {
    Cell top = pop();
    Cell next = pop();
    push(next % top);
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
    printf("%ld ", top);
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
void fth_type(void) {
    Cell len = pop();
    char * ptr = (char*)pop();
    Cell counter = 0;
    assert(len >= 0);
    assert(ptr != NULL);
    for(;counter < len; ++counter) {
        printf("%c", ptr[counter]);
    }
}
void fth_abs(void) {
    Cell top = pop();
    top *= ((top < 0) * -1) + ((top >= 0) * 1);
    assert(top >= 0);
    push(top);
}
void fth_cr(void) {
    printf("\n");
}
void fth_bl(void) {
    printf(" ");
}
void fth_cell_plus(void) {
    Cell top = pop();
    top += sizeof(Cell);
    push(top);
}
void fth_cells(void) {
    Cell top = pop();
    top *= sizeof(Cell);
    push(top);
}
void fth_nip(void) {
    Cell top = pop();
    pop();
    push(top);
}
void fth_over(void) {
    Cell top = pop();
    Cell next = pop();
    push(next);
    push(top);
    push(next);
}
void fth_rot(void) {
    /* 1 2 3  ->  2 3 1 */
    Cell top = pop();
    Cell next = pop();
    Cell third = pop();
    push(next);
    push(top);
    push(third);
}
void fth_pick(void) {
    Cell top = pop();
    assert(top >= 0);
    assert(stack_len > top);
    {
        Cell index = stack_len - top - 1;
        push(stack[index]);
    }
}
