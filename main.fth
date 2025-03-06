0 constant c_state_compiling
1 constant c_state_interpreting
c_state_compiling value c_state

: c_+ ." fth_add();" cr ;
: c_- ." fth_sub();" cr ;
: c_* ." fth_mul();" cr ;
: c_/ ." fth_div();" cr ;
: c_mod ." fth_mod();" cr ;
: c_emit ." fth_emit();" cr ;
: c_. ." fth_print_top();" cr ;
: c_dup ." fth_dup();" cr ;
: c_swap ." fth_swap();" cr ;
: c_lit ( addr n -- ) ." fth_lit(" type ." );" cr ;
: c_: ( addr n -- ) ." void " type ." (void) {" cr ;
: c_; ( addr n -- ) ." }" cr ;
: c_constant ( n addr n -- ) cr ." #define " type ." = " . cr ;

: record_token ( c-addr n -- ) 
    \ ~
    \ 2dup type
    2dup s" +" compare 0 = if
        \ ~
        c_+
    else 2dup s" -" compare 0 = if
        \ ~
        c_-
    else 2dup s" *" compare 0 = if
        \ ~
        c_*
    else 2dup s" /" compare 0 = if
        \ ~
        c_/
    else 2dup s" mod" compare 0 = if
        \ ~
        c_mod
    else 2dup s" emit" compare 0 = if
        \ ~
        c_emit
    else 2dup s" ." compare 0 = if
        \ ~
        c_.
    else 2dup s" dup" compare 0 = if
        \ ~
        c_dup
    else 2dup s" swap" compare 0 = if
        \ ~
        c_swap
    else 2dup s" :" compare 0 = if
        \ ~
        c_:
    else 2dup s" ;" compare 0 = if
        \ ~
        c_;
    else
        \ ~
        c_lit
    then then then then then then then then then then then
    \ ~
    \ 2dup compare s" constant" if c_+ else
    \ 2dup compare s" lit" if c_lit else
;



: whitespace? ( c -- f )  dup 32 = swap dup 9 = swap dup 10 = swap 13 = or or or ;
: get_input ( -- fileid ) s" test.fth" r/w open-file abort" open-file failed" ;
variable get_c_buf 128 cells allot
: get_c ( fileid -- ch ior ) get_c_buf 1 rot read-file abort" get_c failed" get_c_buf @ swap ;
variable token 256 allot
: init_token_buf token 256 1 cells / erase ; init_token_buf
: cstr_write_char ( c c-addr -- )
    dup dup c@ 1 + swap c!
    dup c@ + c!
;

\ .\" token buf \"" token 16 dump .\" \"" cr
\ char a token cstr_write_char
\ .\" token buf \"" token 16 dump .\" \"" cr
\ char b token cstr_write_char
\ .\" token buf \"" token 16 dump .\" \"" cr
\ char c token cstr_write_char
\ .\" token buf \"" token 16 dump .\" \"" cr
\ token count type cr



: main 
    init_token_buf
    get_input
    begin dup get_c while
        \ dup .\" ch is \"" emit .\" \"" cr
        \ .\" token buf \"" token 16 dump .\" \"" cr
        dup whitespace? if
            drop
            \ ." whitespace" cr
            \ token count type cr
            \ ~
            token count record_token
            init_token_buf
        else
            \ ." before write"
            \ dup .\" ch is \"" emit .\" \"" cr
            \ token 32 dump
            token cstr_write_char 
            \ ." afterwrite"
            \ token 32 dump cr
            \ token count type cr
            \ init_token_buf
        then
    repeat
    ." finished reading file"
    bye
; main




