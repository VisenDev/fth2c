#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define strcap 64

/* TODO
 * Support for create???
 * File io
 * Rework allot and variables to use a runtime stack rather than malloc
 */

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

const char reserved_identifiers[][strcap] = {
    /* C Keywords (C11)*/
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "inline", "int", "long", "register", "restrict", "return", "short",
    "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof",
    "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary", "_Noreturn",
    "_Static_assert", "_Thread_local",

    /* Standard Library Functions (Common headers)*/
    
    /* <stdio.h> */
    "printf", "scanf", "fprintf", "fscanf", "sprintf", "sscanf",
    "vprintf", "vscanf", "vfprintf", "vfscanf", "vsprintf", "vsscanf",
    "fopen", "fclose", "fgets", "fputs", "fputc", "fgetc",
    "fread", "fwrite", "feof", "ferror", "clearerr", "rewind",
    "ftell", "fseek", "fflush", "perror",

    /* <stdlib.h> */
    "malloc", "calloc", "realloc", "free",
    "exit", "abort", "atexit", "system",
    "atoi", "atof", "atol", "strtod", "strtol", "strtoul",
    "rand", "srand", "bsearch", "qsort",

    /* <string.h> */
    "strcpy", "strncpy", "strcat", "strncat", "strcmp", "strncmp",
    "strlen", "strchr", "strrchr", "strstr", "strtok",
    "memcpy", "memmove", "memcmp", "memset",

    /* <math.h> */
    "sin", "cos", "tan", "asin", "acos", "atan", "atan2",
    "sinh", "cosh", "tanh", "exp", "log", "log10",
    "pow", "sqrt", "ceil", "floor", "fabs", "fmod",

    /* <time.h> */
    "clock", "time", "difftime", "mktime", "strftime",

    /* <ctype.h> */
    "isalpha", "isdigit", "isalnum", "islower", "isupper",
    "tolower", "toupper", "isspace", "ispunct", "isprint",
};
const long reserved_identifiers_len =
    sizeof(reserved_identifiers) /
    (sizeof(reserved_identifiers[0]));

/* TODO 
 * replace names that are C keywords*/
char * normalize_identifier(const char * str) {
    static char buf[1024] = {0};
    long buf_i = 0;
    long i = 0;
    memset(buf, 0, sizeof(buf));
    if(isdigit(str[i])) {
        buf[buf_i] = '_';
        ++buf_i;
    }
    for(i = 0; str[i] != 0; ++i, ++buf_i) {
        if(!isalpha(str[i]) && !isdigit(str[i]) && str[i] != '_') {
            if(str[i] == '-') {
                buf[buf_i] = '_';
            } else {
                buf_i += snprintf(buf + buf_i, sizeof(buf) - buf_i, "_%d_", str[i]);
            }
        } else {
            buf[buf_i] = str[i];
        }
    }
    if(find_string(reserved_identifiers, reserved_identifiers_len, buf) != -1) {
        buf[buf_i] = '_';
    }
    return buf;
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
        buf[len] = tolower(ch);
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

FILE * string_to_file(const char * contents) {
    FILE *fp = tmpfile();
    if (!fp) {
        fatal_error("Failed to open a temporary file");
    }
    fputs(contents, fp);
    rewind(fp);
    return fp;
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
    } else if(streql(tok, "1+")) {
        out(mode, "fth_lit(1);\n");
        out(mode, "fth_add();\n");
    } else if(streql(tok, "1-")) {
        out(mode, "fth_lit(1);\n");
        out(mode, "fth_sub();\n");
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
    } else if (streql(tok, "decimal")) {
        out(mode,"fth_decimal();\n"); 
    } else if (streql(tok, "hex")) {
        out(mode,"fth_hex();\n"); 
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
    } else if(streql("?", tok)) {
        out(mode, "push(*(Cell *)pop());\nfth_print_top();\n");
    } else {
        return 0;
    }
    return 1;
}


int match_boolean_operations(char * tok, int toplevel) {
    OutputMode mode = toplevel ? LATER : NOW;
    if(streql("=", tok)) {
        out(mode, "fth_eql();\n");
    } else if(streql("<>", tok)) {
        out(mode, "fth_not_eql();\n");
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
    } else if(streql("drop", tok)) {
        out(mode, "fth_drop();\n");
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
    } else if(streql("key", tok)) {
        out(mode, "push(fgetc(stdin));\n");
    } else if(streql("type", tok)) {
        out(mode, "fth_type();\n");
    } else if(streql("cr", tok)) {
        out(mode, "fth_cr();\n");
    } else if(streql("bl", tok)) {
        out(mode, "fth_bl();\n");
    } else if(streql("space", tok)) {
        out(mode, "fth_space();\n");
    } else {
        return 0;
    }
    return 1;
}

int match_numeric_conversion_operations(char * tok, int toplevel) {
    OutputMode mode = toplevel ? LATER : NOW;
    static int active = 0;

    if(streql("s>d", tok)) {
        out(mode, "fth_single_to_double();\n");
    } else if(streql("<#", tok)) {
        if(active != 0) {
            fatal_error("Nested '<#'\n");
        }
        active = 1;
        out(mode, "fth_begin_numeric_conversion();\n");
    } else if(streql("#>", tok)) {
        if(active != 1) {
            fatal_error("Unstructured '#>'\n");
        }
        active = 0;
        out(mode, "fth_end_numeric_conversion();\n");
    } else if(streql("#", tok)) {
        if(active != 1) {
            fatal_error("Unstructured '#' outside of <#   #> delimiters\n");
        }
        out(mode, "fth_convert_digit();\n");
    } else if(streql("#s", tok)) {
        if(active != 1) {
            fatal_error("Unstructured '#s' outside of <#   #> delimiters\n");
        }
        out(mode, "fth_convert_number();\n");
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

/*const char * teststr = " : let create , ; ";*/
/* 0 let counter
 * 
 * out(LATER, push(0));
 * if(is_create_word(tok)) {
 *    tok = get_token(fp);
 *    out(NOW, "Cell %s = 0;", tok);
 *    push(create_word_things(tok));
 * }
 *
 */

/* if(streql("create", tok)) {
 *    
 * }*/


typedef enum {
    TOP_LEVEL,
    WORD_NAME,
    WORD_BODY,
    PAREN_COMMENT_TOP_LEVEL,
    PAREN_COMMENT_WORD_BODY,
    VARIABLE_NAME,
    /*SLASH_COMMENT,*/
} CompilerState;

typedef struct {
    char variables[1024][strcap];
    long variables_len;
    char words[1024][strcap];
    long words_len;
    char constants[1024][strcap];
    long constants_len;
    CompilerState state;
} Compiler;

void compile_file(FILE * fp, Compiler * c) {
    char * tok = get_token(fp);

    /*printf("#include \"fth2c.h\"\n");*/
    /*print_file("fth2c.h");*/

    for(;tok != NULL; tok = get_token(fp)) {
        switch(c->state) { 
        case TOP_LEVEL:

            if(find_string(c->words, c->words_len, tok) != -1) {
                tok = normalize_identifier(tok);
                out(LATER, "%s();\n", tok);
            } else if(find_string(c->variables, c->variables_len, tok) != -1) {
                tok = normalize_identifier(tok);
                out(LATER, "fth_lit((Cell)%s);\n", tok);
            } else if(find_string(c->constants, c->constants_len, tok) != -1) {
                tok = normalize_identifier(tok);
                out(LATER, "fth_lit(%s);\n", tok);
            } else if(match_math_operations(tok, 1)) {
            } else if(match_memory_operations(tok, 1)) {
            } else if(match_boolean_operations(tok, 1)) {
            } else if(match_stack_operations(tok, 1)) {
            } else if(match_io_operations(tok, 1)) {
            } else if(match_numeric_conversion_operations(tok, 1)) {

            /* Include */
            } else if(streql("include", tok)) {
                FILE * included = NULL;
                tok = get_token(fp);
                included = fopen(tok, "r");
                if(included == NULL) {
                    fatal_error("Failed to open file: %s\n", tok);
                }
                compile_file(included, c);
                fclose(included);

            /* Variables */
            } else if(streql("variable", tok)) {
                c->state = VARIABLE_NAME;
            } else if(streql("constant", tok)) {
                tok = get_token(fp);
                memmove(c->constants[c->constants_len], tok, strlen(tok));
                ++c->constants_len;
                tok = normalize_identifier(tok);
                out(NOW, "Cell %s = 0;\n", tok);
                out(LATER, "%s = pop();\n", tok);
            } else if(streql("value", tok)) {
                tok = get_token(fp);
                memmove(c->constants[c->constants_len], tok, strlen(tok));
                ++c->constants_len;
                tok = normalize_identifier(tok);
                out(NOW, "Cell %s = 0;\n", tok);
                out(LATER, "%s = pop();\n", tok);
            } else if(streql("to", tok)) {
                tok = get_token(fp);
                memmove(c->constants[c->constants_len], tok, strlen(tok));
                ++c->constants_len;
                tok = normalize_identifier(tok);
                out(LATER, "%s = pop();\n", tok);
            } else if (streql(tok, ":")) {
                c->state = WORD_NAME;
            } else if (streql(tok, "allot")) {
                if(c->variables_len <= 0) {
                    fatal_error("Tried to allot memory for a non-existent variable"); 
                }
                out(LATER, "%s = realloc(%s, pop() + sizeof(Cell));\n",
                        c->variables[c->variables_len - 1],
                        c->variables[c->variables_len - 1]);
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
                c->state = PAREN_COMMENT_TOP_LEVEL;

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
            if(find_string(c->words, c->words_len, tok) != -1) {
                fatal_error("Multiply defined symbol: %s\n", tok);
            }
            memmove(c->words[c->words_len], tok, strlen(tok));
            c->words_len += 1;
            tok =normalize_identifier(tok);
            out(NOW, "void %s(void) {\n", tok);
            c->state = WORD_BODY;
            break;
        case PAREN_COMMENT_TOP_LEVEL:
            if(strlen(tok) > 0 && tok[strlen(tok) - 1] == ')') {
                c->state = TOP_LEVEL;
            }
            break;
        case PAREN_COMMENT_WORD_BODY:
            if(strlen(tok) > 0 && tok[strlen(tok) - 1] == ')') {
                c->state = WORD_BODY;
            }
            break;
        case WORD_BODY: 
            if(find_string(c->words, c->words_len, tok) != -1) {
                tok =normalize_identifier(tok);
                out(NOW,"%s();\n", tok);
            } else if(find_string(c->variables, c->variables_len, tok) != -1) {
                tok =normalize_identifier(tok);
                out(NOW, "fth_lit((Cell)%s);\n", tok);
            } else if(find_string(c->constants, c->constants_len, tok) != -1) {
                tok = normalize_identifier(tok);
                out(NOW, "fth_lit(%s);\n", tok);
            } else if(match_math_operations(tok, 0)) {
            } else if(match_memory_operations(tok, 0)) {
            } else if(match_boolean_operations(tok, 0)) {
            } else if(match_stack_operations(tok, 0)) {
            } else if(match_io_operations(tok, 0)) {
            } else if(match_numeric_conversion_operations(tok, 0)) {

            } else if (streql(tok, ".\"")) {
                char buf[1024] = {0};
                long buflen = 0;
                char ch = fgetc(fp);
                for(;ch != '"' && !feof(fp); ch = fgetc(fp), ++buflen) {
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

            /* Immediate words */
            } else if(streql("[char]", tok)) {
                tok = get_token(fp);
                out(NOW, "push('%c');\n", tok[0]);

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
                c->state = PAREN_COMMENT_WORD_BODY;

            /* to */
            } else if(streql("to", tok)) {
                tok = get_token(fp);
                memmove(c->constants[c->constants_len], tok, strlen(tok));
                ++c->constants_len;
                tok = normalize_identifier(tok);
                out(NOW, "%s = pop();\n", tok);

            /* Misc */
            } else if(streql("bye", tok)) {
                printf("exit(0);\n");
            } else if(streql(";", tok)) {
                printf("}\n");
                c->state = TOP_LEVEL;
            } else if(is_number(tok)) {
                printf("fth_lit(%s);\n", tok);
            } else {
                fprintf(stderr, "invalid tok: \"%s\"\n", tok);
            }
            break;
            
        case VARIABLE_NAME:
            if(find_string(c->words, c->words_len, tok) != -1) {
                fatal_error("Multiply defined symbol: %s\n", tok);
            }
            memmove(c->variables[c->variables_len], tok, strlen(tok));
            c->variables_len += 1;
            tok = normalize_identifier(tok);
            printf("Cell * %s = 0;\n", tok);
            out(LATER, "%s = malloc(sizeof(Cell));\n", tok);
            c->state = TOP_LEVEL;

        }
    }

    /*
    printf("int main(void) {\n");
    out(FLUSH, NULL);
    printf("return 0;\n}\n");
    */
}

void compiliation_start(FILE * fp) {
    Compiler c = {0};
    print_file("fth2c.h");

    compile_file(fp, &c);
    
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
        compiliation_start(fp);
        fclose(fp);
    }

    return 0;
}
