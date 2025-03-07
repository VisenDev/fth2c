#include "fth2c.h"
void square(void) {
fth_dup();
fth_mul();
}
Cell hello = 0;
void test(void) {
fth_lit(1);
fth_lit(2);
fth_lit(3);
fth_lit(4);
fth_add();
fth_add();
fth_print_top();
fth_print_top();
}
void print_hello(void) {
fth_lit((Cell)&hello);
fth_print_top();
}
void say_hello(void) {
fth_lit(5);
fth_less_than();
if(pop()) {
print_hello();
}
}
int main(void) {
test();
fth_lit(10);
square();
square();
fth_print_top();
fth_lit(52);
fth_emit();
fth_lit(4);
fth_lit(10);
fth_emit();
print_hello();

return 0;
}
