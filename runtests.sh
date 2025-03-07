#!/bin/sh

SRC=tests/*.fth
DIR=build

make fth2c
mkdir -p $DIR

echo "\n\n"

for FILE in $SRC; do
    echo "Testing diff for $FILE..."
    ./$DIR/fth2c $FILE 2>/dev/null > build/test.c
    cc build/test.c -fsanitize=address,undefined -g -o build/test.run
    ./build/test.run > build/test.out 2>/dev/null
    if [ $? -ne 0 ]; then
        # echo "nvim build/test.c" | sh
        break
    fi
    gforth $FILE > build/answer.out
    diff build/test.out build/answer.out
    echo "Done"
done
     
