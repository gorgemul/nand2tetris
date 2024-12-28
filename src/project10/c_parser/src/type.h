#ifndef TYPE_H
#define TYPE_H

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

#endif
