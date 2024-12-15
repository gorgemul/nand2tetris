#ifndef TYPE_H
#define TYPE_H

#define NO_ARG2 -1
#define TEMP_REGISTER "R13"

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
    Command cmd;
    char arg1[50];
    int arg2;
} Statement;

#endif
