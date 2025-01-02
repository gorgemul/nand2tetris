#include "vm_writer.h"
#include "compile_engine.h"
#include "tokenizer.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SymbolTable *g_st = NULL;
VarKind g_kind = NOT_VAR;
char g_type = ' '; // first char of type: <int | char | boolean | className>
char g_current_class_type[MAX_VAR_NAME];
int g_is_dec = 0;

char *resolve_kind_name(VarKind kind, Token *token)
{
    char *buf = malloc(sizeof(*buf) * MAX_VAR_NAME);

    switch (kind) {
    case STATIC:
        strcat(buf, "static");
        break;
    case FIELD:
        strcat(buf, "field");
        break;
    case ARG:
        strcat(buf, "argument");
        break;
    case VAR:
        strcat(buf, "var");
        break;
    case NOT_VAR:
        (token->value[0] >= 'A' && token->value[0] <= 'Z')
            ? strcat(buf, "class")
            : strcat(buf, "subroutine");
        break;
    }

    return buf;
}

char peek_next_token_first_char(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    static Token token = {0};

    if (has_more_token(i_stream)) get_token(i_stream, &token);

    fseek(i_stream, og_pos, SEEK_SET);

    return token.value[0];
}

Statement peek_next_statement(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    Token token = {0};
    Statement s = NO_MORE_STATEMENT;

    if (has_more_token(i_stream)) get_token(i_stream, &token);

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

    if (has_more_token(i_stream)) get_token(i_stream, &token);
    if (has_more_token(i_stream)) get_token(i_stream, &token);

    fseek(i_stream, og_pos, SEEK_SET);

    return token.value[0];
}

int has_more_class_var_dec(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    Token token = {0};

    if (has_more_token(i_stream)) get_token(i_stream, &token);

    fseek(i_stream, og_pos, SEEK_SET);

    return (strcmp(token.value, "static") == 0)
        || (strcmp(token.value, "field") == 0);
}

int has_more_subroutine_dec(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    Token token = {0};

    if (has_more_token(i_stream)) get_token(i_stream, &token);

    fseek(i_stream, og_pos, SEEK_SET);

    return (strcmp(token.value, "constructor") == 0)
        || (strcmp(token.value, "function") == 0)
        || (strcmp(token.value, "method") == 0);
}

int has_more_var_dec(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    Token token = {0};

    if (has_more_token(i_stream)) get_token(i_stream, &token);

    fseek(i_stream, og_pos, SEEK_SET);

    return strcmp(token.value, "var") == 0;
}

int has_else_condition(FILE *i_stream)
{
    long og_pos = ftell(i_stream);
    Token token = {0};

    if (has_more_token(i_stream)) get_token(i_stream, &token);

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
/* char *escape_char(char c) */
/* { */
/*     char *buf = malloc(sizeof(*buf) * 16); */

/*     switch (c) { */
/*     case '<': */
/*         strcat(buf, "&lt;"); */
/*         break; */
/*     case '>': */
/*         strcat(buf, "&gt;"); */
/*         break; */
/*     case '&': */
/*         strcat(buf, "&amp;"); */
/*         break; */
/*     default: */
/*         buf[0] = c; */
/*         buf[1] = '\0'; */
/*         break; */
/*     } */

/*     return buf; */
/* } */

int is_primitive_type(Token *token)
{
    if (strcmp(token->value, "int") == 0 ||
        strcmp(token->value, "char") == 0 ||
        strcmp(token->value, "boolean") == 0
    ) {
        return 1;
    }

    return 0;
}

void write_identifier(FILE *o_stream, Token *token)
{
    int is_var_dec = (g_kind != NOT_VAR);
    char usage[16] = {0};
    char *kind_str = NULL;
    int is_class_type = is_var_dec && (token->value[0] >= 'A' && token->value[0] <= 'Z') && (token->value[0] == g_type);

    if (is_class_type) {
        fprintf(o_stream, "<class-used> %s </class-used>\n", token->value);
        strcpy(g_current_class_type, token->value);
        return;
    }

    strcat(usage, g_is_dec ? "dec" : "used");
    kind_str = resolve_kind_name(g_kind, token);

    if (is_var_dec) {
        int index = get_var_count(g_st, g_kind);

        add_var(g_st, token->value, g_current_class_type, g_kind);
        fprintf(o_stream, "<%s-%d-%s> %s </%s-%d-%s>\n", kind_str, index, usage, token->value, kind_str, index, usage);
    } else {
        int index = get_var_index(g_st, token->value);

        if (index == -1) {
            fprintf(o_stream, "<%s-%s> %s </%s-%s>\n", kind_str, usage, token->value, kind_str, usage);
        } else {
            char *new_kind_str = resolve_kind_name(get_var_kind(g_st, token->value), token);
            fprintf(o_stream, "<%s-%d-%s> %s </%s-%d-%s>\n", new_kind_str, index, usage, token->value, new_kind_str, index, usage);
            free(new_kind_str);
        }
    }

    free(kind_str);
}

void compile_token(FILE *o_stream, Token *token)
{
    switch (token->type) {
    case KEYWORD:
        /* fprintf_with_depth(o_stream, "<keyword> %s </keyword>\n", token->value); */
        /* if (is_primitive_type(token)) strcpy(g_current_class_type, token->value); */
        return;
    case SYMBOL:
        /* fprintf_with_depth(o_stream, "<symbol> %s </symbol>\n", buf); */
        return;
    case IDENTIFIER:
        write_identifier(o_stream, token);
        return;
    case INT_CONST:
        write_push(o_stream, SEG_CONST, atoi(token->value));
        return;
    case STRING_CONST:
        return;
    }
}

void advance(FILE *i_stream)
{
    static Token token = {0};
    if (has_more_token(i_stream)) get_token(i_stream, &token);
}

// Only handle handle '-' sub case
Operator get_op_type(char op)
{
    switch (op) {
    case '+':
        return ADD;
    case '-':
        return SUB;
    case '=':
        return EQ;
    case '>':
        return GT;
    case '<':
        return LT;
    case '&':
        return AND;
    case '|':
        return OR;
    case '~':
        return NOT;
    default:
        return -1;
    }
}

Segment get_segment_type(char *var_name)
{
    VarKind kind = get_var_kind(g_st, var_name);

    if (kind == STATIC) return SEG_STATIC;
    if (kind == FIELD)  return SEG_THIS;
    if (kind == ARG)    return SEG_ARG;
    if (kind == VAR)    return SEG_LOCAL;

    return -1;
}

char *get_function_name(char *name)
{
    static char buf[0xFF] = {0};

    memset(buf, 0, sizeof(buf));

    strcat(buf, g_st->class_name);
    strcat(buf, ".");
    strcat(buf, name);

    return buf;
}

char *get_call_function_name(char *obj, char *subroutine_call)
{
    static char buf[0xFF] = {0};
    memset(buf, 0, sizeof(buf));

    if (obj) {
        strcat(buf, obj);
        strcat(buf, ".");
        strcat(buf, subroutine_call);
    } else {
        strcat(buf, subroutine_call);
    }

    return buf;
}

void write_syscall_op(FILE *o_stream, char op)
{
    fprintf(o_stream, "call Math.%s 2\n", op == '*' ? "multiply" : "divide");
}

// NOTE: Maybe change it to vm writer only tokens
void compile_next_token(FILE *i_stream, FILE *o_stream)
{
    static Token token = {0};
    if (has_more_token(i_stream)) get_token(i_stream, &token);
    compile_token(o_stream, &token);
}

void compile_expression_list(FILE *i_stream, FILE *o_stream, int *exp_counter)
{
    if (peek_next_token_first_char(i_stream) == ')') return;

    (*exp_counter)++;
    compile_expression(i_stream, o_stream);

    while (peek_next_token_first_char(i_stream) == ',') {
        (*exp_counter)++;
        advance(i_stream); // ,
        compile_expression(i_stream, o_stream);
    }
}

void compile_term(FILE *i_stream, FILE *o_stream)
{
    // handle unary operator
    if (has_more_operator(i_stream)) {
        char unaryOperator = peek_next_token_first_char(i_stream);
        advance(i_stream); // unaryOperator
        compile_term(i_stream, o_stream);

        if (unaryOperator == '-')
            write_arithmetic(o_stream, NEG);
        else
            write_arithmetic(o_stream, NOT);

        return;
    }

    if (peek_next_token_first_char(i_stream) == '(') {
        advance(i_stream); // (
        compile_expression(i_stream, o_stream);
        advance(i_stream); // )
        return;
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
        compile_expression_list(i_stream, o_stream, NULL);
        compile_next_token(i_stream, o_stream); // )
        break;
    case '.':
        compile_next_token(i_stream, o_stream); // subroutineName | className | varName
        compile_next_token(i_stream, o_stream); // .
        compile_next_token(i_stream, o_stream); // subroutineName
        compile_next_token(i_stream, o_stream); // (
        compile_expression_list(i_stream, o_stream, NULL);
        compile_next_token(i_stream, o_stream); // )
        break;
    default:
        compile_next_token(i_stream, o_stream); // everything else
        break;
    }
}

void compile_expression(FILE *i_stream, FILE *o_stream)
{
    compile_term(i_stream, o_stream);
    while (has_more_operator(i_stream)) {
        char operator = peek_next_token_first_char(i_stream);
        int is_primitive_op = (operator != '*') && (operator != '/');

        advance(i_stream); // operator
        compile_term(i_stream, o_stream);

        if (is_primitive_op)
            write_arithmetic(o_stream, get_op_type(operator));
        else
            write_syscall_op(o_stream, operator);
    }
}

void compile_let_statement(FILE *i_stream, FILE *o_stream)
{
    Token token = {0};

    advance(i_stream); // let
    if (has_more_token(i_stream)) get_token(i_stream, &token); // varName
    if (peek_next_token_first_char(i_stream) == '[') { // TODO:
        compile_next_token(i_stream, o_stream); // [
        compile_expression(i_stream, o_stream);
        compile_next_token(i_stream, o_stream); // ]
    }
    advance(i_stream); // =
    compile_expression(i_stream, o_stream);
    write_pop(o_stream, get_segment_type(token.value), get_var_index(g_st, token.value));
    advance(i_stream); // ;
}

void compile_if_statement(FILE *i_stream, FILE *o_stream)
{
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
}

void compile_while_statement(FILE *i_stream, FILE *o_stream)
{
    compile_next_token(i_stream, o_stream); // while
    compile_next_token(i_stream, o_stream); // (
    compile_expression(i_stream, o_stream);
    compile_next_token(i_stream, o_stream); // )
    compile_next_token(i_stream, o_stream); // {
    compile_statements(i_stream, o_stream);
    compile_next_token(i_stream, o_stream); // }
}

void compile_do_statement(FILE *i_stream, FILE *o_stream)
{
    int exp_counter = 0;
    Token token1 = {0};
    Token token2 = {0};

    advance(i_stream); // do

    if (has_more_token(i_stream)) get_token(i_stream, &token1); // subroutineName | className | varName

    switch (peek_next_token_first_char(i_stream)) {
    case '(':
        advance(i_stream); // (
        compile_expression_list(i_stream, o_stream, &exp_counter);
        advance(i_stream); // )
        write_call(o_stream, get_call_function_name(NULL, token1.value), exp_counter);
        break;
    case '.':
        advance(i_stream); // .
        if (has_more_token(i_stream)) get_token(i_stream, &token2); // subroutineName
        advance(i_stream); // (
        compile_expression_list(i_stream, o_stream, &exp_counter);
        advance(i_stream); // )
        write_call(o_stream, get_call_function_name(token1.value, token2.value), exp_counter);
        break;
    default:
        break;
    }

    write_pop(o_stream, SEG_TEMP, 0);
    advance(i_stream); // ;
}

void compile_return_statement(FILE *i_stream, FILE *o_stream)
{
    advance(i_stream); // return

    if (peek_next_token_first_char(i_stream) != ';')
        compile_expression(i_stream, o_stream);
    else
        write_push(o_stream, SEG_CONST, 0);

    write_return(o_stream);
    advance(i_stream); // ;
}

void compile_statements(FILE *i_stream, FILE *o_stream)
{
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
}

void compile_var_dec(FILE *i_stream, FILE *o_stream)
{
    g_kind = VAR;
    g_type = peek_next_next_token_first_char(i_stream);
    g_is_dec = 1;

    advance(i_stream); // var
    advance(i_stream); // type
    compile_next_token(i_stream, o_stream); // varName
    while (peek_next_token_first_char(i_stream) == ',') {
        advance(i_stream); // ,
        compile_next_token(i_stream, o_stream); // varName
    }
    advance(i_stream); // ;

    g_is_dec = 0;
    g_kind = NOT_VAR;
}

void compile_parameter_list(FILE *i_stream, FILE *o_stream, int *param_counter)
{
    if (peek_next_token_first_char(i_stream) == ')') goto end;

    g_kind = ARG;
    g_type = peek_next_token_first_char(i_stream);
    g_is_dec = 1;
    (*param_counter)++;
    advance(i_stream); // type
    advance(i_stream); // varName
    while (peek_next_token_first_char(i_stream) == ',') {
        (*param_counter)++;

        advance(i_stream); // ,
        g_type = peek_next_token_first_char(i_stream);
        advance(i_stream); // type
        compile_next_token(i_stream, o_stream); // varName
    }

end:
    g_is_dec = 0;
    g_kind = NOT_VAR;
}

void compile_subroutine_body(FILE *i_stream, FILE *o_stream)
{
    advance(i_stream); // {
    while (has_more_var_dec(i_stream)) compile_var_dec(i_stream, o_stream);
    compile_statements(i_stream, o_stream);
    advance(i_stream); // }
}

void compile_subroutine_dec(FILE *i_stream, FILE *o_stream)
{
    Token token = {0};
    int param_counter = 0;
    g_type = peek_next_next_token_first_char(i_stream);
    g_is_dec = 1;

    advance(i_stream); // constructor | function | method
    advance(i_stream); // void | type

    if (has_more_token(i_stream)) get_token(i_stream, &token); // subroutineName

    advance(i_stream); // (
    compile_parameter_list(i_stream, o_stream, &param_counter);
    advance(i_stream); // )
    write_function(o_stream, get_function_name(token.value), param_counter);

    g_is_dec = 0;
    compile_subroutine_body(i_stream, o_stream);

    destruct_subroutine_vars(g_st);
}

void compile_class_var_dec(FILE *i_stream, FILE *o_stream)
{
    g_kind = peek_next_token_first_char(i_stream) == 's' ? STATIC : FIELD;
    g_type = peek_next_next_token_first_char(i_stream);
    g_is_dec = 1;

    advance(i_stream);  // static | field
    advance(i_stream);  // type
    compile_next_token(i_stream, o_stream); // varName
    while (peek_next_token_first_char(i_stream) == ',') {
        advance(i_stream); // ,
        compile_next_token(i_stream, o_stream); // varName
    }

    advance(i_stream); // ';'

    g_is_dec = 0;
    g_kind = NOT_VAR;
}

/** entry point for compile engine */
void compile_class(FILE *i_stream, FILE *o_stream)
{
    g_st = init_symbol_table(i_stream);

    advance(i_stream); // class
    advance(i_stream); // className
    advance(i_stream); // {
    while (has_more_class_var_dec(i_stream)) compile_class_var_dec(i_stream, o_stream);
    while (has_more_subroutine_dec(i_stream)) compile_subroutine_dec(i_stream, o_stream);
    advance(i_stream); // }

    destruct_symbol_table(g_st);
    g_st = NULL;
}
