#include "compile_engine.h"
#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    LET,
    IF,
    WHILE,
    DO,
    RETURN,
    NO_MORE_STATEMENT = -1,
} Statement;

int g_depth = 0;

char peek_next_token_first_char(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    static Token token = {0};

    if (has_more_token(i_stream)) getToken(i_stream, &token);

    fseek(i_stream, og_pos, SEEK_SET);

    return token.value[0];
}

void fprintf_with_depth(FILE *o_stream, char *format_str, char *str)
{
    int indent_num = g_depth * 2;
    char indent_str[64] = {0};

    memset(indent_str, ' ', indent_num);
    indent_str[indent_num] = '\0';

    fprintf(o_stream, "%s", indent_str);
    fprintf(o_stream, format_str, str);
}

Statement peek_next_statement(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    Token token = {0};
    Statement s = NO_MORE_STATEMENT;

    if (has_more_token(i_stream)) getToken(i_stream, &token);

    fseek(i_stream, og_pos, SEEK_SET);

    if (strcmp(token.value, "let") == 0) {
        s = LET;
    } else if (strcmp(token.value, "if") == 0) {
        s = IF;
    } else if (strcmp(token.value, "while") == 0) {
        s = WHILE;
    } else if (strcmp(token.value, "do") == 0) {
        s = DO;
    } else if (strcmp(token.value, "return") == 0) {
        s = RETURN;
    }

    return s;
}

char peek_next_next_token_first_char(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    Token token = {0};

    if (has_more_token(i_stream)) getToken(i_stream, &token);
    if (has_more_token(i_stream)) getToken(i_stream, &token);

    fseek(i_stream, og_pos, SEEK_SET);

    return token.value[0];
}

int has_more_class_var_dec(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    Token token = {0};

    if (has_more_token(i_stream)) getToken(i_stream, &token);

    fseek(i_stream, og_pos, SEEK_SET);

    return (strcmp(token.value, "static") == 0)
        || (strcmp(token.value, "field") == 0);
}

int has_more_subroutine_dec(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    Token token = {0};

    if (has_more_token(i_stream)) getToken(i_stream, &token);

    fseek(i_stream, og_pos, SEEK_SET);

    return (strcmp(token.value, "constructor") == 0)
        || (strcmp(token.value, "function") == 0)
        || (strcmp(token.value, "method") == 0);
}

int has_more_var_dec(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    Token token = {0};

    if (has_more_token(i_stream)) getToken(i_stream, &token);

    fseek(i_stream, og_pos, SEEK_SET);

    return strcmp(token.value, "var") == 0;
}

int has_else_condition(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    Token token = {0};

    if (has_more_token(i_stream)) getToken(i_stream, &token);

    fseek(i_stream, og_pos, SEEK_SET);

    return strcmp(token.value, "else") == 0;
}

int has_more_operator(FILE *i_stream)
{
    switch (peek_next_token_first_char(i_stream)) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '&':
    case '|':
    case '<':
    case '>':
    case '=':
    case '~':
        return 1;
    default:
        return 0;
    }
}

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

void compile_token(FILE *o_stream, Token *token)
{
    char *buf = NULL;

    switch (token->type) {
    case KEYWORD:
        fprintf_with_depth(o_stream, "<keyword> %s </keyword>\n", token->value);
        return;
    case SYMBOL:
        buf = escape_char(token->value[0]);
        fprintf_with_depth(o_stream, "<symbol> %s </symbol>\n", buf);
        free(buf);
        return;
    case IDENTIFIER:
        fprintf_with_depth(o_stream, "<identifier> %s </identifier>\n", token->value);
        return;
    case INT_CONST:
        fprintf_with_depth(o_stream, "<integerConstant> %s </integerConstant>\n", token->value);
        return;
    case STRING_CONST:
        fprintf_with_depth(o_stream, "<stringConstant> %s </stringConstant>\n", token->value);
        return;
    }
}

void compile_next_token(FILE *i_stream, FILE *o_stream)
{
    static Token token = {0};
    if (has_more_token(i_stream)) getToken(i_stream, &token);
    compile_token(o_stream, &token);
}

void compile_expression_list(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<expressionList>\n", NULL);
    g_depth++;

    if (peek_next_token_first_char(i_stream) == ')') goto end;

    compile_expression(i_stream, o_stream);

    while (peek_next_token_first_char(i_stream) == ',') {
        compile_next_token(i_stream, o_stream); // ,
        compile_expression(i_stream, o_stream);
    }

end:
    g_depth--;
    fprintf_with_depth(o_stream, "</expressionList>\n", NULL);
}

void compile_term(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<term>\n", NULL);
    g_depth++;

    if (has_more_operator(i_stream)) {
        compile_next_token(i_stream, o_stream); // unaryOperator
        compile_term(i_stream, o_stream);
        goto end;
    }

    if (peek_next_token_first_char(i_stream) == '(') {
        compile_next_token(i_stream, o_stream); // (
        compile_expression(i_stream, o_stream);
        compile_next_token(i_stream, o_stream); // )
        goto end;
    }

    switch (peek_next_next_token_first_char(i_stream)) {
    case '[':
        compile_next_token(i_stream, o_stream); // varName
        compile_next_token(i_stream, o_stream); // [
        compile_expression(i_stream, o_stream);
        compile_next_token(i_stream, o_stream); // ]
        break;
    case '(':
        compile_next_token(i_stream, o_stream); // subroutineName | className | varName
        compile_next_token(i_stream, o_stream); // (
        compile_expression_list(i_stream, o_stream);
        compile_next_token(i_stream, o_stream); // )
        break;
    case '.':
        compile_next_token(i_stream, o_stream); // subroutineName | className | varName
        compile_next_token(i_stream, o_stream); // .
        compile_next_token(i_stream, o_stream); // subroutineName
        compile_next_token(i_stream, o_stream); // (
        compile_expression_list(i_stream, o_stream);
        compile_next_token(i_stream, o_stream); // )
        break;
    default:
        compile_next_token(i_stream, o_stream); // everything else
        break;
    }

end:
    g_depth--;
    fprintf_with_depth(o_stream, "</term>\n", NULL);
}

void compile_expression(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<expression>\n", NULL);
    g_depth++;

    compile_term(i_stream, o_stream);
    while (has_more_operator(i_stream)) {
        compile_next_token(i_stream, o_stream); // operator
        compile_term(i_stream, o_stream);
    }

    g_depth--;
    fprintf_with_depth(o_stream, "</expression>\n", NULL);
}

void compile_let_statement(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<letStatement>\n", NULL);
    g_depth++;

    compile_next_token(i_stream, o_stream); // let
    compile_next_token(i_stream, o_stream); // varName
    if (peek_next_token_first_char(i_stream) == '[') {
        compile_next_token(i_stream, o_stream); // [
        compile_expression(i_stream, o_stream);
        compile_next_token(i_stream, o_stream); // ]
    }
    compile_next_token(i_stream, o_stream); // =
    compile_expression(i_stream, o_stream);
    compile_next_token(i_stream, o_stream); // ;

    g_depth--;
    fprintf_with_depth(o_stream, "</letStatement>\n", NULL);
}

void compile_if_statement(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<ifStatement>\n", NULL);
    g_depth++;

    compile_next_token(i_stream, o_stream); // if
    compile_next_token(i_stream, o_stream); // (
    compile_expression(i_stream, o_stream);
    compile_next_token(i_stream, o_stream); // )
    compile_next_token(i_stream, o_stream); // {
    compile_statements(i_stream, o_stream);
    compile_next_token(i_stream, o_stream); // }
    if (has_else_condition(i_stream)) {
        compile_next_token(i_stream, o_stream); // else
        compile_next_token(i_stream, o_stream); // {
        compile_statements(i_stream, o_stream);
        compile_next_token(i_stream, o_stream); // }
    }

    g_depth--;
    fprintf_with_depth(o_stream, "</ifStatement>\n", NULL);
}

void compile_while_statement(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<whileStatement>\n", NULL);
    g_depth++;

    compile_next_token(i_stream, o_stream); // while
    compile_next_token(i_stream, o_stream); // (
    compile_expression(i_stream, o_stream);
    compile_next_token(i_stream, o_stream); // )
    compile_next_token(i_stream, o_stream); // {
    compile_statements(i_stream, o_stream);
    compile_next_token(i_stream, o_stream); // }

    g_depth--;
    fprintf_with_depth(o_stream, "</whileStatement>\n", NULL);
}

void compile_do_statement(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<doStatement>\n", NULL);
    g_depth++;

    compile_next_token(i_stream, o_stream); // do
    compile_next_token(i_stream, o_stream); // subroutineName | className | varName

    switch (peek_next_token_first_char(i_stream)) {
    case '(':
        compile_next_token(i_stream, o_stream); // (
        compile_expression_list(i_stream, o_stream);
        compile_next_token(i_stream, o_stream); // )
        break;
    case '.':
        compile_next_token(i_stream, o_stream); // .
        compile_next_token(i_stream, o_stream); // subroutineName
        compile_next_token(i_stream, o_stream); // (
        compile_expression_list(i_stream, o_stream);
        compile_next_token(i_stream, o_stream); // )
        break;
    default:
        break;
    }

    compile_next_token(i_stream, o_stream); // ;

    g_depth--;
    fprintf_with_depth(o_stream, "</doStatement>\n", NULL);
}

void compile_return_statement(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<returnStatement>\n", NULL);
    g_depth++;

    compile_next_token(i_stream, o_stream); // return
    if (peek_next_token_first_char(i_stream) != ';') compile_expression(i_stream, o_stream);
    compile_next_token(i_stream, o_stream); // ;

    g_depth--;
    fprintf_with_depth(o_stream, "</returnStatement>\n", NULL);
}

void compile_statements(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<statements>\n", NULL);
    g_depth++;

    Statement s = peek_next_statement(i_stream);

    while ((s = peek_next_statement(i_stream)) != NO_MORE_STATEMENT) {
        switch (s) {
        case LET:
            compile_let_statement(i_stream, o_stream);
            break;
        case IF:
            compile_if_statement(i_stream, o_stream);
            break;
        case WHILE:
            compile_while_statement(i_stream, o_stream);
            break;
        case DO:
            compile_do_statement(i_stream, o_stream);
            break;
        case RETURN:
            compile_return_statement(i_stream, o_stream);
            break;
        default:
            break;
        }
    }

    g_depth--;
    fprintf_with_depth(o_stream, "</statements>\n", NULL);
}

void compile_var_dec(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<varDec>\n", NULL);
    g_depth++;

    compile_next_token(i_stream, o_stream); // var
    compile_next_token(i_stream, o_stream); // type
    compile_next_token(i_stream, o_stream); // varName
    while (peek_next_token_first_char(i_stream) == ',') {
        compile_next_token(i_stream, o_stream); // ,
        compile_next_token(i_stream, o_stream); // varName
    }
    compile_next_token(i_stream, o_stream); // ;

    g_depth--;
    fprintf_with_depth(o_stream, "</varDec>\n", NULL);
}

void compile_parameter_list(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<parameterList>\n", NULL);
    g_depth++;

    if (peek_next_token_first_char(i_stream) == ')') goto end;

    compile_next_token(i_stream, o_stream); // type
    compile_next_token(i_stream, o_stream); // varName
    while (peek_next_token_first_char(i_stream) == ',') {
        compile_next_token(i_stream, o_stream); // ,
        compile_next_token(i_stream, o_stream); // type
        compile_next_token(i_stream, o_stream); // varName
    }

end:
    g_depth--;
    fprintf_with_depth(o_stream, "</parameterList>\n", NULL);
}

void compile_subroutine_body(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<subroutineBody>\n", NULL);
    g_depth++;

    compile_next_token(i_stream, o_stream); // {
    while (has_more_var_dec(i_stream)) compile_var_dec(i_stream, o_stream);
    compile_statements(i_stream, o_stream);
    compile_next_token(i_stream, o_stream); // }

    g_depth--;
    fprintf_with_depth(o_stream, "</subroutineBody>\n", NULL);
}

void compile_subroutine_dec(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<subroutineDec>\n", NULL);
    g_depth++;

    compile_next_token(i_stream, o_stream); // constructor | function | method
    compile_next_token(i_stream, o_stream); // void | type
    compile_next_token(i_stream, o_stream); // subroutineName
    compile_next_token(i_stream, o_stream); // (
    compile_parameter_list(i_stream, o_stream);
    compile_next_token(i_stream, o_stream); // )
    compile_subroutine_body(i_stream, o_stream);

    g_depth--;
    fprintf_with_depth(o_stream, "</subroutineDec>\n", NULL);
}

void compile_class_var_dec(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<classVarDec>\n", NULL);
    g_depth++;

    compile_next_token(i_stream, o_stream); // static | field
    compile_next_token(i_stream, o_stream); // type
    compile_next_token(i_stream, o_stream); // varName
    while (peek_next_token_first_char(i_stream) == ',') {
        compile_next_token(i_stream, o_stream); // ,
        compile_next_token(i_stream, o_stream); // varName
    }
    compile_next_token(i_stream, o_stream); // ';'

    g_depth--;
    fprintf_with_depth(o_stream, "</classVarDec>\n", NULL);
}

/** entry point for compile engine */
void compile_class(FILE *i_stream, FILE *o_stream)
{
    fprintf_with_depth(o_stream, "<class>\n", NULL);
    g_depth++;

    compile_next_token(i_stream, o_stream); // class
    compile_next_token(i_stream, o_stream); // className
    compile_next_token(i_stream, o_stream); // {
    while (has_more_class_var_dec(i_stream)) compile_class_var_dec(i_stream, o_stream);
    while (has_more_subroutine_dec(i_stream)) compile_subroutine_dec(i_stream, o_stream);
    compile_next_token(i_stream, o_stream); // }

    g_depth--;
    fprintf_with_depth(o_stream, "</class>", NULL);
}
