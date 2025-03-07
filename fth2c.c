#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define strcap 64

void fatal_error(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    abort();
}

int is_number(char * str) {
    while(*str != 0) {
        if(!isdigit(*str)) {
            return 0; 
        }
        ++str;
    }
    return 1;
}

int is_identifier(char * str) {
    if(!isalpha(*str) && *str != '_') {
        return 0; 
    }
    while(*str != 0) {
        if(!isalpha(*str) && *str != '_' && !isdigit(*str)) {
            return 0;
        }
        ++str;
    }
    return 1;
}

void normalize_identifier(char * str) {
    while(*str != 0) {
        if(!isalpha(*str) && !isdigit(*str) && *str != '_') {
            *str = '_';
        }
        ++str;
    }
}

char * get_token(FILE * fp) {
    static char buf[1024] = {0};
    int len = 0;
    char ch = fgetc(fp);

    /*skip whitespace*/
    while(isspace(ch)) {
        ch = fgetc(fp);
    }
    while(!feof(fp) && !isspace(ch)) {
        buf[len] = ch;
        buf[len + 1] = 0;
        ++len;
        ch = fgetc(fp);
    }

    if(feof(fp)) {
        return NULL;
    } else {
        return buf;
    }
}

int streql(const char * a, const char * b) {
    return strcmp(a, b) == 0;
}


void deferred_printf(const char * fmt, ...) {
    static char buf[100000] = {0};
    static long len = 0;
    va_list args;

    if(fmt == NULL) {
        printf("%s\n", buf);
        memset(buf, 0, sizeof(buf));
        len = 0;
    } else {
        va_start(args, fmt);
        len += vsnprintf(buf + len, sizeof(buf), fmt, args);
        va_end(args);
    }
}

int find_string(const char haystack[][strcap], long count, const char * needle) {
    long i = 0;
    for(i = 0; i < count; ++i) {
        if(streql(haystack[i], needle)) {
            return i;
        }
    }
    return -1;
}

void print_file(const char * filename) {
    FILE* fp = fopen(filename, "r");
    char ch;
    if(fp == NULL) {
        fatal_error("Failed to open: %s\n", filename);
    }
    while ((ch = fgetc(fp)) != EOF) {
        putchar(ch);
    }
    fclose(fp);
}

void run_compiler(FILE * fp) {
    char * tok = get_token(fp);
    enum {
        TOP_LEVEL,
        WORD_NAME,
        WORD_BODY,
        PAREN_COMMENT_TOP_LEVEL,
        PAREN_COMMENT_WORD_BODY,
        VARIABLE_NAME,
        /*SLASH_COMMENT,*/
    } state = 0;
    static char variables[1024][strcap] = {0};
    long variables_len = 0;
    static char words[1024][strcap] = {0};
    long words_len = 0;

    /*printf("#include \"fth2c.h\"\n");*/
    print_file("fth2c.h");

    for(;tok != NULL; tok = get_token(fp)) {
        switch(state) { 
        case TOP_LEVEL:

            /*Math operations*/
            if(streql(tok, "+")) {
                deferred_printf("fth_add();\n");
            } else if (streql(tok, "-")) {
                deferred_printf("fth_sub();\n");
            } else if(streql("*", tok)) {
                deferred_printf("fth_mul();\n");
            } else if(streql("/", tok)) {
                deferred_printf("fth_div();\n");
            } else if(streql("mod", tok)) {
                deferred_printf("fth_mod();\n");
            } else if(streql("dup", tok)) {

            /* Memory manipulation */
            } else if(streql("!", tok)) {
                deferred_printf("tmp = pop();\n");
                deferred_printf("*(Cell *)tmp = pop();\n");
            } else if(streql("+!", tok)) {
                deferred_printf("tmp = pop();\n");
                deferred_printf("*(Cell *)tmp += pop();\n");
            } else if(streql("@", tok)) {
                deferred_printf("push(*(Cell *)pop());\n");

            /* Boolean Logic */
            } else if(streql("=", tok)) {
                deferred_printf("fth_eql();\n");
            } else if(streql("<", tok)) {
                deferred_printf("fth_less_than();\n");
            } else if(streql(">", tok)) {
                deferred_printf("fth_greater_than();\n");
            } else if(streql("<=", tok)) {
                deferred_printf("fth_less_than_eql();\n");
            } else if(streql(">=", tok)) {
                deferred_printf("fth_greater_than_eql();\n");
            } else if(streql("and", tok)) {
                deferred_printf("fth_and();\n");
            } else if(streql("or", tok)) {
                deferred_printf("fth_or();\n");

            /*Stack manipulation*/
                deferred_printf("fth_dup();\n");
            } else if(streql("swap", tok)) {
                deferred_printf("fth_swap();\n");
            } else if(streql("emit", tok)) {

            /* IO */
                deferred_printf("fth_emit();\n");
            } else if(streql(".", tok)) {
                deferred_printf("fth_print_top();\n");

            /* Variables */
            } else if(streql("variable", tok)) {
                state = VARIABLE_NAME;
            } else if (streql(tok, ":")) {
                state = WORD_NAME;
            } else if (streql(tok, "cells")) {
                deferred_printf("push(pop() * sizeof(Cell));\n"); 
            } else if (streql(tok, "allot")) {
                if(variables_len <= 0) {
                    fatal_error("Tried to allot memory for a non-existent variable"); 
                }
                deferred_printf("%s = realloc(%s, pop() + sizeof(Cell));\n",
                        variables[variables_len - 1],
                        variables[variables_len - 1]);


            /* Comments */
            } else if (streql(tok, "(")) {
                state = PAREN_COMMENT_TOP_LEVEL;

            /* Misc */
            } else if(streql("bye", tok)) {
                deferred_printf("exit(0);\n");
            } else if(is_number(tok)) {
                deferred_printf("fth_lit(%s);\n", tok);
            } else if(find_string(words, words_len, tok) != -1) {
                normalize_identifier(tok);
                deferred_printf("%s();\n", tok);
            } else if(find_string(variables, variables_len, tok) != -1) {
                normalize_identifier(tok);
                deferred_printf("fth_lit((Cell)%s);\n", tok);
            } else {
                printf("invalid tok: \"%s\"\n", tok);
            }
            break;
        case WORD_NAME: 
            if(find_string(words, words_len, tok) != -1) {
                fatal_error("Multiply defined symbol: %s\n", tok);
            }
            memmove(words[words_len], tok, strlen(tok));
            words_len += 1;
            normalize_identifier(tok);
            printf("void %s(void) {\n Cell tmp = 0;\n", tok);
            state = WORD_BODY;
            break;
        case PAREN_COMMENT_TOP_LEVEL:
            if(strlen(tok) > 0 && tok[strlen(tok) - 1] == ')') {
                state = TOP_LEVEL;
            }
            break;
        case PAREN_COMMENT_WORD_BODY:
            if(strlen(tok) > 0 && tok[strlen(tok) - 1] == ')') {
                state = WORD_BODY;
            }
            break;
        case WORD_BODY: 



            /* Math */
            if(streql("+", tok)) {
                printf("fth_add();\n");
            } else if(streql("-", tok)) {
                printf("fth_sub();\n");
            } else if(streql("*", tok)) {
                printf("fth_mul();\n");
            } else if(streql("/", tok)) {
                printf("fth_div();\n");
            } else if(streql("mod", tok)) {
                printf("fth_mod();\n");

            /* Memory manipulation */
            } else if(streql("!", tok)) {
                printf("tmp = pop();\n");
                printf("*(Cell *)tmp = pop();\n");
            } else if(streql("+!", tok)) {
                deferred_printf("tmp = pop();\n");
                deferred_printf("*(Cell *)tmp += pop();\n");
            } else if(streql("@", tok)) {
                printf("push(*(Cell *)pop());\n");
            } else if (streql(tok, "cells")) {
                printf("push(pop() * sizeof(Cell));\n"); 

            /* Boolean Logic */
            } else if(streql("=", tok)) {
                printf("fth_eql();\n");
            } else if(streql("<", tok)) {
                printf("fth_less_than();\n");
            } else if(streql(">", tok)) {
                printf("fth_greater_than();\n");
            } else if(streql("<=", tok)) {
                printf("fth_less_than_eql();\n");
            } else if(streql(">=", tok)) {
                printf("fth_greater_than_eql();\n");
            } else if(streql("and", tok)) {
                printf("fth_and();\n");
            } else if(streql("or", tok)) {
                printf("fth_or();\n");

            /* Stack manipulation */
            } else if(streql("dup", tok)) {
                printf("fth_dup();\n");
            } else if(streql("swap", tok)) {
                printf("fth_swap();\n");
            } else if(streql("mod", tok)) {
                printf("fth_mod();\n");
            } else if(streql("emit", tok)) {
                printf("fth_emit();\n");
            } else if(streql(".", tok)) {
                printf("fth_print_top();\n");

            /* if statements */
            } else if(streql("if", tok)) {
                printf("if(pop()) {\n");
            } else if(streql("else", tok)) {
                printf("} else {\n");
            } else if(streql("then", tok)) {
                printf("}\n");


            /* Comments */
            } else if (streql(tok, "(")) {
                state = PAREN_COMMENT_WORD_BODY;

            /* Misc */
            } else if(streql("bye", tok)) {
                printf("exit(0);\n");
            } else if(streql(";", tok)) {
                printf("}\n");
                state = TOP_LEVEL;
            } else if(is_number(tok)) {
                printf("fth_lit(%s);\n", tok);
            } else if(find_string(words, words_len, tok) != -1) {
                normalize_identifier(tok);
                printf("%s();\n", tok);
            } else if(find_string(variables, variables_len, tok) != -1) {
                normalize_identifier(tok);
                printf("fth_lit((Cell)%s);\n", tok);
            } else {
                printf("invalid tok: \"%s\"\n", tok);
            }
            break;
            
        case VARIABLE_NAME:
            if(find_string(words, words_len, tok) != -1) {
                fatal_error("Multiply defined symbol: %s\n", tok);
            }
            memmove(variables[variables_len], tok, strlen(tok));
            variables_len += 1;
            normalize_identifier(tok);
            printf("Cell * %s = 0;\n", tok);
            deferred_printf("%s = malloc(sizeof(Cell));\n", tok);
            state = TOP_LEVEL;

        }
    }

    printf("int main(void) {\nCell tmp = 0;\n");
    deferred_printf(NULL);
    printf("return 0;\n}\n");
}

int main(int argc, char ** argv) {
    (void)argc;
    (void)argv;

    if(argc < 2) {
        fatal_error("expected filename as argument");
    } else {
        char * filename = argv[1];
        FILE * fp = fopen(filename, "rw");
        run_compiler(fp);
        fclose(fp);
    }

    return 0;
}
