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
    vfprintf(stderr, fmt, args);
    va_end(args);
    abort();
}

int is_number(char * str) {
    if(*str == '-') {
        ++str;
    }
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

/* TODO 
 * replace names that are C keywords*/
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

int find_string(const char haystack[][strcap], long count, const char * needle) {
    long i = 0;
    for(i = count - 1; i >= 0; --i) {
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

typedef enum {NOW, LATER, FLUSH} OutputMode;
void out(OutputMode mode, char * fmt, ...) {
    static char buf[100000] = {0};
    static long len = 0;
    va_list args;

    if(mode == NOW) {
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    } else if(mode == LATER) {
        va_start(args, fmt);
        len += vsnprintf(buf + len, sizeof(buf), fmt, args);
        va_end(args);
    } else if(mode == FLUSH) {
        printf("%s\n", buf);
        memset(buf, 0, sizeof(buf));
        len = 0;
    }
}

/*
void indent(int modifier) {
    static int level = 0;
    int i = 0;
    if(modifier != 0) {
        level += modifier;
    } else {
        for(i = 0; i < level; ++i) {
            printf("    ");
        }
    }
}
*/

/* returns one if a match is found */
int match_math_operations(char * tok, int toplevel) {
    OutputMode mode = toplevel ? LATER : NOW;
    if(streql(tok, "+")) {
        out(mode, "fth_add();\n");
    } else if (streql(tok, "-")) {
        out(mode, "fth_sub();\n");
    } else if(streql("*", tok)) {
        out(mode,"fth_mul();\n");
    } else if(streql("/", tok)) {
        out(mode, "fth_div();\n");
    } else if(streql("mod", tok)) {
        out(mode, "fth_mod();\n");
    } else if(streql("abs", tok)) {
        out(mode, "fth_abs();\n");
    } else if (streql(tok, "cells")) {
        out(mode,"fth_cells();\n"); 
    } else if (streql(tok, "cell+")) {
        out(mode,"fth_cell_plus();\n"); 
    } else {
        return 0;
    }
    return 1;
}

int match_memory_operations(char * tok, int toplevel) {
    OutputMode mode = toplevel ? LATER : NOW;
    if(streql("!", tok)) {
        out(mode, "tmp = pop();\n");
        out(mode, "*(Cell *)tmp = pop();\n");
    } else if(streql("+!", tok)) {
        out(mode,"tmp = pop();\n");
        out(mode,"*(Cell *)tmp += pop();\n");
    } else if(streql("@", tok)) {
        out(mode, "push(*(Cell *)pop());\n");
    } else {
        return 0;
    }
    return 1;
}


int match_boolean_operations(char * tok, int toplevel) {
    OutputMode mode = toplevel ? LATER : NOW;
    if(streql("=", tok)) {
        out(mode, "fth_eql();\n");
    } else if(streql("<", tok)) {
        out(mode, "fth_less_than();\n");
    } else if(streql(">", tok)) {
        out(mode, "fth_greater_than();\n");
    } else if(streql("<=", tok)) {
        out(mode, "fth_less_than_eql();\n");
    } else if(streql(">=", tok)) {
        out(mode, "fth_greater_than_eql();\n");
    } else if(streql("and", tok)) {
        out(mode, "fth_and();\n");
    } else if(streql("or", tok)) {
        out(mode, "fth_or();\n");
    } else {
        return 0;
    }
    return 1;
}

int match_stack_operations(char * tok, int toplevel) {
    OutputMode mode = toplevel ? LATER : NOW;

    if(streql("dup", tok)) {
        out(mode, "fth_dup();\n");
    } else if(streql("swap", tok)) {
        out(mode, "fth_swap();\n");
    } else if(streql("nip", tok)) {
        out(mode, "fth_nip();\n");
    } else if(streql("rot", tok)) {
        out(mode, "fth_rot();\n");
    } else if(streql("pick", tok)) {
        out(mode, "fth_pick();\n");
    } else if(streql("over", tok)) {
        out(mode, "fth_over();\n");
    } else {
        return 0;
    }
    return 1;
}

int match_io_operations(char * tok, int toplevel) {
    OutputMode mode = toplevel ? LATER : NOW;

    if(streql("emit", tok)) {
        out(mode, "fth_emit();\n");
    } else if(streql(".", tok)) {
        out(mode, "fth_print_top();\n");
    } else if(streql("type", tok)) {
        out(mode, "fth_type();\n");
    } else if(streql("cr", tok)) {
        out(mode, "fth_cr();\n");
    } else if(streql("bl", tok)) {
        out(mode, "fth_bl();\n");
    } else {
        return 0;
    }
    return 1;
}

/*TODO actually compile these*/
const char * builtins = 
    ": cr ( -- ) 10 emit ; "
    ": abs (n -- n) dup 0 > if else -1 * then ; "
;

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

            if(find_string(words, words_len, tok) != -1) {
                normalize_identifier(tok);
                out(LATER, "%s();\n", tok);
            } else if(find_string(variables, variables_len, tok) != -1) {
                normalize_identifier(tok);
                out(LATER, "fth_lit((Cell)%s);\n", tok);
            } else if(match_math_operations(tok, 1)) {
            } else if(match_memory_operations(tok, 1)) {
            } else if(match_boolean_operations(tok, 1)) {
            } else if(match_stack_operations(tok, 1)) {
            } else if(match_io_operations(tok, 1)) {

            /* Variables */
            } else if(streql("variable", tok)) {
                state = VARIABLE_NAME;
            } else if (streql(tok, ":")) {
                state = WORD_NAME;
            } else if (streql(tok, "allot")) {
                if(variables_len <= 0) {
                    fatal_error("Tried to allot memory for a non-existent variable"); 
                }
                out(LATER, "%s = realloc(%s, pop() + sizeof(Cell));\n",
                        variables[variables_len - 1],
                        variables[variables_len - 1]);
            /* Strings */
            } else if (streql(tok, ".\"")) {
                char buf[1024] = {0};
                long buflen = 0;
                char ch = fgetc(fp);
                for(;ch != '"' && !feof(fp); ch = fgetc(fp), ++buflen) {
                    buf[buflen] = ch;
                    buf[buflen + 1] = 0;
                }
                out(LATER, "printf(\"%s\");\n", buf);
            } else if (streql(tok, "s\"")) {
                char buf[1024] = {0};
                long buflen = 0;
                char ch = fgetc(fp);
                for(;ch != '"' && !feof(fp); ch = fgetc(fp), ++buflen) {
                    buf[buflen] = ch;
                    buf[buflen + 1] = 0;
                }
                out(LATER, "fth_lit((Cell)\"%s\");\n", buf);
                out(LATER, "fth_lit(%d);\n", strlen(buf));


            /* Comments */
            } else if (streql(tok, "(")) {
                state = PAREN_COMMENT_TOP_LEVEL;

            /* Misc */
            } else if(streql("bye", tok)) {
                out(LATER, "exit(0);\n");
            } else if(is_number(tok)) {
                out(LATER, "fth_lit(%s);\n", tok);
            } else {
                fprintf(stderr, "invalid tok: \"%s\"\n", tok);
            }
            break;
        case WORD_NAME: 
            if(find_string(words, words_len, tok) != -1) {
                fatal_error("Multiply defined symbol: %s\n", tok);
            }
            memmove(words[words_len], tok, strlen(tok));
            words_len += 1;
            normalize_identifier(tok);
            out(NOW, "void %s(void) {\n", tok);
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
            if(find_string(words, words_len, tok) != -1) {
                normalize_identifier(tok);
                printf("%s();\n", tok);
            } else if(find_string(variables, variables_len, tok) != -1) {
                normalize_identifier(tok);
                printf("fth_lit((Cell)%s);\n", tok);
            } else if(match_math_operations(tok, 0)) {
            } else if(match_memory_operations(tok, 0)) {
            } else if(match_boolean_operations(tok, 0)) {
            } else if(match_stack_operations(tok, 0)) {
            } else if(match_io_operations(tok, 0)) {

            } else if (streql(tok, ".\"")) {
                char buf[1024] = {0};
                long buflen = 0;
                char ch = fgetc(fp);
                for(;ch != '"' && !feof(fp); ch = fgetc(fp)) {
                    buf[buflen] = ch;
                    buf[buflen + 1] = 0;
                }
                out(NOW, "printf(\"%s\");\n", buf);
            } else if (streql(tok, "s\"")) {
                char buf[1024] = {0};
                long buflen = 0;
                char ch = fgetc(fp);
                for(;ch != '"' && !feof(fp); ch = fgetc(fp), ++buflen) {
                    buf[buflen] = ch;
                    buf[buflen + 1] = 0;
                }
                out(NOW, "fth_lit((Cell)\"%s\");\n", buf);
                out(NOW, "fth_lit(%d);\n", strlen(buf));

            /* if statements */
            } else if(streql("if", tok)) {
                out(NOW, "if(pop()) {\n");
            } else if(streql("else", tok)) {
                out(NOW, "} else {\n");
            } else if(streql("then", tok)) {
                out(NOW, "}\n");

            /* begin while repeat */
            } else if(streql("begin", tok)) {
                out(NOW, "do {\n");
            } else if(streql("while", tok)) {
                out(NOW, "if(!pop()) break;\n");
            } else if(streql("repeat", tok)) {
                out(NOW, "} while(1);\n");
            } else if(streql("until", tok)) {
                out(NOW, "} while(!pop());\n");

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
            } else {
                fprintf(stderr, "invalid tok: \"%s\"\n", tok);
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
            out(LATER, "%s = malloc(sizeof(Cell));\n", tok);
            state = TOP_LEVEL;

        }
    }

    printf("int main(void) {\n");
    out(FLUSH, NULL);
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
