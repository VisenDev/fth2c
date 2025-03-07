: square dup * ;

: test
    1 2 3 4 + + . . 
;
test 10 square square . 52 emit
