DIR=build
FTH2C=./$(DIR)/fth2c

fth2c: fth2c.c fth2c.h
	mkdir -p $(DIR)
	cc fth2c.c -fsanitize=address,undefined -g -std=c89 -Wall -Wextra -o $(FTH2C)

.PHONY test:
test:
	./runtests.sh

.PHONY clean:
clean: 
	trash $(DIR)
