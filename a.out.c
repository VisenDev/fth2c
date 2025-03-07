#include "fth2c.h"
void square(void) {
fth_dup();
fth_mul();
}
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
int main(void) {
test();
fth_lit(10);
square();
square();
fth_print_top();
fth_lit(52);
fth_emit();

return 0;
}
