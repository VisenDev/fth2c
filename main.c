#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>


void fatal_error(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    abort();
}

int isnumber(char * str) {
    while(*str != 0) {
        if(!isdigit(*str)) {
            return 0; 
        }
        ++str;
    }
    return 1;
}

int isidentifier(char * str) {
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
        if(!isalpha(str) && !isdigit(*str) && *str != '_') {
            *str = '_';
        }
        ++str;
    }
}

int main(int argc, char ** argv) {
    (void)argc;
    (void)argv;

    if(argc < 2) {
        fatal_error("expected filename as argument");
    } else {
        char * filename = argv[1];
        FILE * fp = fopen(filename, "rw");
        char ch = fgetc(fp);
        char tok[1024] = {0};
        char runtime_code[2048] = {0};
        long runtime_code_len = 0;
        long toklen = 0;
        enum {
            STATE_START,
            STATE_WORD_START,
            STATE_WORD_BODY,
        } state = STATE_START;

        printf("#include \"fth2c.h\"\n\n");

        while(!feof(fp)) {
            if(isspace(ch)) {
                if(toklen != 0) {
                    toklen = 0;
                    if(state == STATE_START) {
                        if(strcmp(":", tok) == 0) {
                            printf("void ");
                            state = STATE_WORD_START;
                        } else if(isidentifier(tok)) {
                            runtime_code_len +=
                            sprintf(runtime_code + runtime_code_len, "%s();\n", tok);
                        } else {
                            printf("invalid tok: \"%s\"\n", tok);
                        }
                    } else if(state == STATE_WORD_START) {
                        if(!isidentifier(tok)) {
                            normalize_identifier(tok);
                        }
                        printf("%s", tok);
                        state = STATE_WORD_BODY;
                        printf("(void) {\n");
                    } else if(state == STATE_WORD_BODY) {
                        if(strcmp("+", tok) == 0) {
                            printf("fth_add();\n");
                        } else if(strcmp("-", tok) == 0) {
                            printf("fth_sub();\n");
                        } else if(strcmp("*", tok) == 0) {
                            printf("fth_mul();\n");
                        } else if(strcmp("/", tok) == 0) {
                            printf("fth_div();\n");
                        } else if(strcmp("mod", tok) == 0) {
                            printf("fth_mod();\n");
                        } else if(strcmp("emit", tok) == 0) {
                            printf("fth_emit();\n");
                        } else if(strcmp(".", tok) == 0) {
                            printf("fth_print_top();\n");
                        } else if(strcmp(";", tok) == 0) {
                            printf("}\n");
                            state = STATE_START;
                        } else if(isnumber(tok)) {
                            printf("fth_lit(%s);\n", tok);
                        } else {
                            printf("invalid tok: \"%s\"\n", tok);
                        }
                    }
                }

            } else {
                tok[toklen] = ch;
                tok[toklen + 1] = 0;
                toklen += 1;
            }
            ch = fgetc(fp);
        }

        printf("int main(void) {\n");
        printf("%s\n", runtime_code);
        printf("return 0;\n}\n");

        fclose(fp);
    }

    return 0;
}
