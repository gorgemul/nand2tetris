#include "compile_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Since we are using xml, '<' AND '>' AND '&' would be escape
char *escape_char(char c)
{
    char *buf = malloc(sizeof(*buf) * 16);

    switch (c) {
    case '<':
        strcat(buf, "&lt;");
        break;
    case '>':
        strcat(buf, "&gt;");
        break;
    case '&':
        strcat(buf, "&amp;");
        break;
    default:
        buf[0] = c;
        buf[1] = '\0';
        break;
    }

    return buf;
}

void compile_term(FILE *o_stream, Token *token)
{
    char *buf = NULL;

    switch (token->type) {
    case KEYWORD:
        fprintf(o_stream, "<keyword> %s </keyword>\n", token->value);
        return;
    case SYMBOL:
        buf = escape_char(token->value[0]);
        fprintf(o_stream, "<symbol> %s </symbol>\n", buf);
        free(buf);
        return;
    case IDENTIFIER:
        fprintf(o_stream, "<identifier> %s </identifier>\n", token->value);
        return;
    case INT_CONST:
        fprintf(o_stream, "<integerConstant> %s </integerConstant>\n", token->value);
        return;
    case STRING_CONST:
        fprintf(o_stream, "<stringConstant> %s </stringConstant>\n", token->value);
        return;
    }
}
