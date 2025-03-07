variable x
variable y

: run-the-test ( -- )
    5 x !
    begin
    x @ 0 > while
        5 y !
        begin
        y @ 0 > while
            102 emit
            -1 y +!
        repeat
        -1 x +!
    repeat
;

5 run-the-test bye
    
