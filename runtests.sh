#!/bin/sh

SRC=tests/*.fth
DIR=build

make fth2c
mkdir -p $DIR

echo "\n\n"

for FILE in $SRC; do
    ./$DIR/fth2c $FILE 2>/dev/null > build/test.c
    cc build/test.c -fsanitize=address,undefined -g -o build/test.run
    ./build/test.run > build/test.out 2>/dev/null
    gforth $FILE > build/answer.out
    echo "Testing diff for $FILE..."
    diff build/test.out build/answer.out
    echo "Done"
done
     
