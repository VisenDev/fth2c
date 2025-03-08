5 5 + 10 - 5 2 * + constant newline-ascii

: print-10-newlines 
    10
    begin
        newline-ascii emit
    1 - dup 0 < until
    ." Done"
    newline-ascii emit
;

print-10-newlines bye
