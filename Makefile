all: a.out
	./a.out

fth2c: fth2c.c fth2c.h
	cc fth2c.c -fsanitize=address -fsanitize=undefined -g -std=c89 -Wall -Wextra -o fth2c

a.out.c: test.fth fth2c
	./fth2c	test.fth > a.out.c

a.out: a.out.c
	cc a.out.c -fsanitize=address,undefined -g -o a.out

.PHONY clean:
clean:
	trash *.out *.dSYM fth2c a.out.c 2>/dev/null
