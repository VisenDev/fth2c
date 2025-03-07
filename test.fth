: square dup * ;

variable hello

: test
    1 2 3 4 + + . . 
;

: print-hello
    hello .
;

: say-hello ( n -- )
    5 < if 
        print-hello
    then
;
    

test 10 square square . 52 emit

4 10 emit print-hello
