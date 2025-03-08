123 value foo
400 value bar

: print-foo-bar
    foo .
    bar .
    foo
    1 to foo
    foo .
    to foo
;

print-foo-bar
980 to bar
print-foo-bar
981 to foo
print-foo-bar
bye
