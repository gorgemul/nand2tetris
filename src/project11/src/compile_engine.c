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

int g_field_counter = 0;
int g_branching_counter = 0;

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

Segment get_segment_type(char *var_name)
{
    VarKind kind = get_var_kind(g_st, var_name);

    if (kind == STATIC) return SEG_STATIC;
    if (kind == FIELD)  return SEG_THIS;
    if (kind == ARG)    return SEG_ARG;
    if (kind == VAR)    return SEG_LOCAL;

    return -1;
}

SubroutineType get_subroutine_type(char c)
{
    switch (c) {
    case 'c':
        return CONSTRUCTOR;
    case 'f':
        return FUNCTION;
    case 'm':
        return METHOD;
    default:
        return -1;
    }
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

int is_keyword_const(char *keyword)
{
    if (strcmp(keyword, "true") == 0 ||
        strcmp(keyword, "false") == 0 ||
        strcmp(keyword, "null") == 0 ||
        strcmp(keyword, "this") == 0
    ) {
        return 1;
    }

    return 0;
}

void write_constructor(FILE *o_stream)
{
    write_push(o_stream, SEG_CONST, g_field_counter);
    write_call(o_stream, "Memory.alloc", 1);
    write_pop(o_stream, SEG_POINTER, 0);
}

void write_method(FILE *o_stream)
{
    write_push(o_stream, SEG_ARG, 0);
    write_pop(o_stream, SEG_POINTER, 0);
}

void write_keyword_constant(FILE *o_stream, char *keyword)
{
    if (strcmp(keyword, "true") == 0) {
        write_push(o_stream, SEG_CONST, 0);
        write_arithmetic(o_stream, NOT);
    } else if (strcmp(keyword, "false") == 0) {
        write_push(o_stream, SEG_CONST, 0);
    } else if (strcmp(keyword, "null") == 0) { // TODO:
    } else if (strcmp(keyword, "this") == 0) {
        write_push(o_stream, SEG_POINTER, 0);
    }
}

void write_identifier(FILE *o_stream, Token *token)
{
    int is_var_dec = (g_kind != NOT_VAR);
    int is_class_type = is_var_dec && (token->value[0] >= 'A' && token->value[0] <= 'Z') && (token->value[0] == g_type);

    if (is_class_type) {
        strcpy(g_current_class_type, token->value);
        return;
    }

    if (is_var_dec) {
        add_var(g_st, token->value, g_current_class_type, g_kind);
    } else {
        int index = get_var_index(g_st, token->value);
        if (index >= 0) write_push(o_stream, get_segment_type(token->value), index);
    }
}

void write_string_const(FILE *o_stream, Token *token)
{
    int str_len = strlen(token->value);

    write_push(o_stream, SEG_CONST, str_len);
    write_call(o_stream, "String.new", 1);

    for (int i = 0; i < str_len; i++) {
        write_push(o_stream, SEG_CONST, token->value[i]);
        write_call(o_stream, "String.appendChar", 2);
    }
}

void compile_token(FILE *o_stream, Token *token)
{
    switch (token->type) {
    case KEYWORD:
        /* fprintf_with_depth(o_stream, "<keyword> %s </keyword>\n", token->value); */
        if (is_primitive_type(token)) {
            strcpy(g_current_class_type, token->value);
        } else if (is_keyword_const(token->value)) {
            write_keyword_constant(o_stream, token->value);
        }
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
        write_string_const(o_stream, token);
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

char *resolve_function_name(char *name)
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

char *get_label_name(char *name, int index)
{
    static char buf[0xFF] = {0};

    memset(buf, 0, sizeof(buf));

    sprintf(buf, "%s#%d", name, index);

    return buf;
}

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

    Token token1 = {0};
    Token token2 = {0};
    int exp_counter = 0;
    int var_index = 0;

    switch (peek_next_next_token_first_char(i_stream)) {
    case '[':
        compile_next_token(i_stream, o_stream); // varName
        advance(i_stream); // [
        compile_expression(i_stream, o_stream);
        advance(i_stream); // ]
        write_arithmetic(o_stream, ADD);
        write_pop(o_stream, SEG_POINTER, 1);
        write_push(o_stream, SEG_THAT, 0);
        break;
    case '(':
        if (has_more_token(i_stream)) get_token(i_stream, &token1); // subroutineName | className | varName
        write_push(o_stream, SEG_POINTER, 0);
        advance(i_stream); // (
        compile_expression_list(i_stream, o_stream, &exp_counter);
        advance(i_stream); // )
        write_call(o_stream, get_call_function_name(g_st->class_name, token1.value), exp_counter+1);
        break;
    case '.':
        if (has_more_token(i_stream)) get_token(i_stream, &token1); // className | varName
        var_index = get_var_index(g_st, token1.value);
        if (var_index != -1) write_push(o_stream, get_segment_type(token1.value), var_index);
        advance(i_stream); // .
        if (has_more_token(i_stream)) get_token(i_stream, &token2); // subroutineName
        advance(i_stream); // (
        compile_expression_list(i_stream, o_stream, &exp_counter);
        advance(i_stream); // )
        if (var_index != -1) {
            write_call(o_stream, get_call_function_name(get_var_type(g_st, token1.value), token2.value), exp_counter+1);
        } else {
            write_call(o_stream, get_call_function_name(token1.value, token2.value), exp_counter);
        }
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

    if (peek_next_token_first_char(i_stream) == '[') {
        compile_token(o_stream, &token);
        advance(i_stream); // [
        compile_expression(i_stream, o_stream);
        advance(i_stream); // ]
        write_arithmetic(o_stream, ADD);
        advance(i_stream); // =

        if (peek_next_next_token_first_char(i_stream) == '[') {
            compile_next_token(i_stream, o_stream);
            advance(i_stream); // [
            compile_expression(i_stream, o_stream);
            advance(i_stream); // ]
            write_arithmetic(o_stream, ADD);
            write_pop(o_stream, SEG_POINTER, 1);
            write_push(o_stream, SEG_THAT, 0);
        } else {
            compile_expression(i_stream, o_stream);
        }

        write_pop(o_stream, SEG_TEMP, 0);
        write_pop(o_stream, SEG_POINTER, 1);
        write_push(o_stream, SEG_TEMP, 0);
        write_pop(o_stream, SEG_THAT, 0);
    } else {
        advance(i_stream); // =
        if (peek_next_next_token_first_char(i_stream) == '[') {
            compile_next_token(i_stream, o_stream);
            advance(i_stream); // [
            compile_expression(i_stream, o_stream);
            advance(i_stream); // ]
            write_arithmetic(o_stream, ADD);
            write_pop(o_stream, SEG_POINTER, 1);
            write_push(o_stream, SEG_THAT, 0);
        } else {
            compile_expression(i_stream, o_stream);
        }

        write_pop(o_stream, get_segment_type(token.value), get_var_index(g_st, token.value));
    }

    advance(i_stream); // ;
}

void compile_if_statement(FILE *i_stream, FILE *o_stream)
{
    int index = ++g_branching_counter;
    advance(i_stream); // if
    advance(i_stream); // (
    compile_expression(i_stream, o_stream);
    advance(i_stream); // )
    write_if(o_stream, get_label_name("IF_TRUE", index));
    write_goto(o_stream, get_label_name("IF_FALSE", index));
    write_label(o_stream, get_label_name("IF_TRUE", index));
    advance(i_stream); // {
    compile_statements(i_stream, o_stream);
    advance(i_stream); // }
    if (has_else_condition(i_stream)) {
        write_goto(o_stream, get_label_name("IF_END", index));
        write_label(o_stream, get_label_name("IF_FALSE", index));
        advance(i_stream); // else
        advance(i_stream); // {
        compile_statements(i_stream, o_stream);
        advance(i_stream); // }
        write_label(o_stream, get_label_name("IF_END", index));
    } else {
        write_label(o_stream, get_label_name("IF_FALSE", index));
    }
}

void compile_while_statement(FILE *i_stream, FILE *o_stream)
{
    int index = ++g_branching_counter;
    write_label(o_stream, get_label_name("WHILE_LOOP", index));
    advance(i_stream); // while
    advance(i_stream); // (
    compile_expression(i_stream, o_stream);
    advance(i_stream); // )
    write_arithmetic(o_stream, NOT);
    write_if(o_stream, get_label_name("END_WHILE", index));
    advance(i_stream); // {
    compile_statements(i_stream, o_stream);
    write_goto(o_stream, get_label_name("WHILE_LOOP", index));
    advance(i_stream); // }
    write_label(o_stream, get_label_name("END_WHILE", index));
}

void compile_do_statement(FILE *i_stream, FILE *o_stream)
{
    int var_index = 0;
    int exp_counter = 0;
    Token token1 = {0};
    Token token2 = {0};

    advance(i_stream); // do

    if (has_more_token(i_stream)) get_token(i_stream, &token1); // subroutineName | className | varName

    switch (peek_next_token_first_char(i_stream)) {
    case '(':
        write_push(o_stream, SEG_POINTER, 0);
        advance(i_stream); // (
        compile_expression_list(i_stream, o_stream, &exp_counter);
        advance(i_stream); // )
        write_call(o_stream, get_call_function_name(g_st->class_name, token1.value), exp_counter+1);
        break;
    case '.':
        var_index = get_var_index(g_st, token1.value);
        if (var_index != -1) write_push(o_stream, get_segment_type(token1.value), var_index);
        advance(i_stream); // .
        if (has_more_token(i_stream)) get_token(i_stream, &token2); // subroutineName
        advance(i_stream); // (
        compile_expression_list(i_stream, o_stream, &exp_counter);
        advance(i_stream); // )
        if (var_index != -1) {
            write_call(o_stream, get_call_function_name(get_var_type(g_st, token1.value), token2.value), exp_counter+1);
        } else {
            write_call(o_stream, get_call_function_name(token1.value, token2.value), exp_counter);
        }
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

void compile_var_dec(FILE *i_stream, FILE *o_stream, int *local_var_counter)
{
    g_kind = VAR;
    g_type = peek_next_next_token_first_char(i_stream);
    g_is_dec = 1;

    (*local_var_counter)++;
    advance(i_stream); // var
    compile_next_token(i_stream, o_stream); // type
    compile_next_token(i_stream, o_stream); // varName
    while (peek_next_token_first_char(i_stream) == ',') {
        (*local_var_counter)++;
        advance(i_stream); // ,
        compile_next_token(i_stream, o_stream); // varName
    }
    advance(i_stream); // ;

    g_is_dec = 0;
    g_kind = NOT_VAR;
}

void compile_parameter_list(FILE *i_stream, FILE *o_stream, SubroutineType type)
{
    if (type == METHOD) add_var(g_st, "PLACEHOLDER", g_st->class_name, ARG);
    if (peek_next_token_first_char(i_stream) == ')') goto end;

    g_kind = ARG;
    g_type = peek_next_token_first_char(i_stream);
    g_is_dec = 1;

    compile_next_token(i_stream, o_stream); // type
    compile_next_token(i_stream, o_stream); // varName
    while (peek_next_token_first_char(i_stream) == ',') {
        advance(i_stream); // ,
        g_type = peek_next_token_first_char(i_stream);
        compile_next_token(i_stream, o_stream); // type
        compile_next_token(i_stream, o_stream); // varName
    }

end:
    g_is_dec = 0;
    g_kind = NOT_VAR;
}

void compile_subroutine_body(FILE *i_stream, FILE *o_stream, SubroutineType type , char *func_name)
{
    int local_var_counter = 0;
    advance(i_stream); // {
    while (has_more_var_dec(i_stream)) compile_var_dec(i_stream, o_stream, &local_var_counter);

    write_function(o_stream, resolve_function_name(func_name), local_var_counter);

    if (type == CONSTRUCTOR) write_constructor(o_stream);
    if (type == METHOD) write_method(o_stream);

    compile_statements(i_stream, o_stream);
    advance(i_stream); // }
}

void compile_subroutine_dec(FILE *i_stream, FILE *o_stream)
{
    Token token = {0};
    SubroutineType type = get_subroutine_type(peek_next_token_first_char(i_stream));
    g_type = peek_next_next_token_first_char(i_stream);
    g_is_dec = 1;

    advance(i_stream); // constructor | function | method
    advance(i_stream); // void | type

    if (has_more_token(i_stream)) get_token(i_stream, &token); // subroutineName

    advance(i_stream); // (
    compile_parameter_list(i_stream, o_stream, type);
    advance(i_stream); // )

    g_is_dec = 0;
    compile_subroutine_body(i_stream, o_stream, type, token.value);

    destruct_subroutine_vars(g_st);
}

void compile_class_var_dec(FILE *i_stream, FILE *o_stream)
{
    g_kind = peek_next_token_first_char(i_stream) == 's' ? STATIC : FIELD;
    g_type = peek_next_next_token_first_char(i_stream);
    g_is_dec = 1;

    advance(i_stream);  // static | field
    compile_next_token(i_stream, o_stream); // type
    if (g_kind == FIELD) g_field_counter++;
    compile_next_token(i_stream, o_stream); // varName
    while (peek_next_token_first_char(i_stream) == ',') {
        advance(i_stream); // ,
        if (g_kind == FIELD) g_field_counter++;
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
    g_branching_counter = 0;
    g_field_counter = 0;
    g_st = NULL;
}
