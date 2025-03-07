: square dup * ;

variable hello 10 square cells allot
variable buf 5 cells allot

: cr 10 emit ;

: test
    1 2 3 4 + + . . 
;

: init-buf ( --)
    0 buf !
    0 buf 1 cells + !
    0 buf 2 cells + !
    0 buf 3 cells + !
    0 buf 4 cells + !
;

: print-buf
    0 buf @ .
    0 buf 1 cells + @ .
    0 buf 2 cells + @ . 0 buf 3 cells + @ .
    0 buf 4 cells + @ . 
;

: print-hello
    hello @ .
;

: say-hello ( n -- )
    5 < if 
        print-hello 
    then
;
    
init-buf
print-buf
5 buf !
print-buf cr cr cr

test 10 square square . 52 emit

4 10 emit cr print-hello
