: run-my-test ( times -- )
    begin
    dup 0 > while
        100 emit 10 emit
        1 -
    repeat
;

10 run-my-test bye

        
