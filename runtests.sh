#!/bin/sh

SRC=tests/*.fth
DIR=build
CFLAGS="-fsanitize=address,undefined -g -Werror -Wall -Wextra -pedantic -std=c89"

make fth2c
mkdir -p $DIR

echo "\n\n"

for FILE in $SRC; do
    echo "Testing diff for $FILE..."
    ./$DIR/fth2c $FILE 2>/dev/null > build/test.c
    if [ $? -ne 0 ]; then
        break
    fi
    cc build/test.c $CFLAGS -o build/test.run
    if [ $? -ne 0 ]; then
        break
    fi
    ./build/test.run > build/test.out 2>/dev/null
    if [ $? -ne 0 ]; then
        break
    fi
    gforth $FILE > build/answer.out
    diff build/test.out build/answer.out
    echo "Done"
done
     
