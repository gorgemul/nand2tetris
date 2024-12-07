#ifndef PARSER_H
#define PARSER_H

#define NO_ARG2 -1

typedef enum Command {
    ILLEGAL_CMD = -1,

    C_ARITHMETIC,
    C_PUSH,
    C_POP,

    C_LABEL,
    C_GOTO,
    C_IF,

    C_FUNCTION,
    C_CALL,
    C_RETURN,
} Command;

typedef struct {
    char arg1[50];

    Command cmd;
    int arg2;
} Statement;

Statement *parser(const char *line);

#endif
