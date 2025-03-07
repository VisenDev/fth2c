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

int streql(char * a, char * b) {
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

void run_compiler(FILE * fp) {
    char * tok = get_token(fp);
    enum {
        TOP_LEVEL,
        WORD_NAME,
        WORD_BODY,
        PAREN_COMMENT,
        /*SLASH_COMMENT,*/
    } state = 0;

    printf("#include \"fth2c.h\"\n");

    for(;tok != NULL; tok = get_token(fp)) {
        switch(state) { 
        case TOP_LEVEL:
            if(streql(tok, "+")) {
                deferred_printf("fth_add();\n");
            } else if (streql(tok, "+")) {
                deferred_printf("fth_sub();\n");
            } else if(streql("*", tok)) {
                deferred_printf("fth_mul();\n");
            } else if(streql("/", tok)) {
                deferred_printf("fth_div();\n");
            } else if(streql("mod", tok)) {
                deferred_printf("fth_mod();\n");
            } else if(streql("emit", tok)) {
                deferred_printf("fth_emit();\n");
            } else if(streql(".", tok)) {
                deferred_printf("fth_print_top();\n");
            } else if (streql(tok, ":")) {
                state = WORD_NAME;
            } else if (streql(tok, "(")) {
                state = PAREN_COMMENT;
            } else if(is_number(tok)) {
                deferred_printf("fth_lit(%s);\n", tok);
            } else if(is_identifier(tok)) {
                deferred_printf("%s();\n", tok);
            } else {
                printf("invalid tok: \"%s\"\n", tok);
            }
            break;
        case WORD_NAME: 
            printf("void %s(void) {\n", tok);
            state = WORD_BODY;
            break;
        case PAREN_COMMENT:
            if(streql(tok, ")")) {
                state = WORD_BODY;
            }
            break;
        case WORD_BODY: 
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
            } else if(streql("emit", tok)) {
                printf("fth_emit();\n");
            } else if(streql(".", tok)) {
                printf("fth_print_top();\n");
            } else if(streql(";", tok)) {
                printf("}\n");
                state = TOP_LEVEL;
            } else if(is_number(tok)) {
                printf("fth_lit(%s);\n", tok);
            } else if(is_identifier(tok)) {
                printf("%s();\n", tok);
            } else {
                printf("invalid tok: \"%s\"\n", tok);
            }
            break;
        }
    }

    printf("int main(void) {\n");
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
