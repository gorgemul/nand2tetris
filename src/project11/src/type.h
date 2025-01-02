#ifndef TYPE_H
#define TYPE_H

#define MAX_VAR_NAME 64
#define LINE_MAX_LENGTH  256
#define TOKEN_MAX_LENGTH 128

typedef enum {
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST,
} TokenType;

typedef struct {
    TokenType type;
    char value[TOKEN_MAX_LENGTH];
} Token;

typedef enum {
    NO_MORE_STATEMENT = -1,
    LET,
    IF,
    WHILE,
    DO,
    RETURN,
} Statement;

typedef struct {
    int class_var_count;
    int subroutine_var_count;
    char class_name[32];
    char **class_vars;
    char **subroutine_vars;
} SymbolTable;

typedef enum {
    NOT_VAR = -1,
    STATIC,
    FIELD,

    ARG,
    VAR,
} VarKind;

typedef enum {
    SEG_CONST,
    SEG_ARG,
    SEG_LOCAL,
    SEG_STATIC,
    SEG_THIS,
    SEG_THAT,
    SEG_POINTER,
    SEG_TEMP,
} Segment;

typedef enum {
    ADD,
    SUB,
    NEG,
    EQ,
    GT,
    LT,
    AND,
    OR,
    NOT,
} Operator;

#endif
