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
        long toklen = 0;
        enum {
            STATE_START,
            STATE_WORD_START,
            STATE_WORD_BODY,
        } state = STATE_START;


        while(!feof(fp)) {
            if(isspace(ch)) {
                if(toklen != 0) {
                    toklen = 0;
                    if(state == STATE_START) {
                        if(strcmp(":", tok) == 0) {
                            printf("void ");
                            state = STATE_WORD_START;
                        } else {
                            printf("invalid tok: \"%s\"\n", tok);
                        }
                    } else if(state == STATE_WORD_START) {
                        printf("%s", tok);
                        state = STATE_WORD_BODY;
                        printf("(void) {");
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
                        } else if(strcmp(";", tok) == 0) {
                            printf("}\n");
                            state = STATE_START;
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

        fclose(fp);
    }

    return 0;
}
