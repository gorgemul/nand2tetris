#include "vm_writer.h"
#include <string.h>

char *seg_to_str(Segment seg)
{
    static char buf[0xFF] = {0};

    memset(buf, 0, sizeof(buf));

    switch (seg) {
    case SEG_CONST:
        strcat(buf, "constant");
        break;
    case SEG_ARG:
        strcat(buf, "argument");
        break;
    case SEG_LOCAL:
        strcat(buf, "local");
        break;
    case SEG_STATIC:
        strcat(buf, "static");
        break;
    case SEG_THIS:
        strcat(buf, "this");
        break;
    case SEG_THAT:
        strcat(buf, "that");
        break;
    case SEG_POINTER:
        strcat(buf, "pointer");
        break;
    case SEG_TEMP:
        strcat(buf, "temp");
        break;
    default:
        break;
    }

    return buf;
}

char *op_to_str(Operator op)
{
    static char buf[0xFF] = {0};

    memset(buf, 0, sizeof(buf));

    switch (op) {
    case ADD:
        strcat(buf, "add");
        break;
    case SUB:
        strcat(buf, "sub");
        break;
    case NEG:
        strcat(buf, "neg");
        break;
    case EQ:
        strcat(buf, "eq");
        break;
    case GT:
        strcat(buf, "gt");
        break;
    case LT:
        strcat(buf, "lt");
        break;
    case AND:
        strcat(buf, "and");
        break;
    case OR:
        strcat(buf, "or");
        break;
    case NOT:
        strcat(buf, "not");
        break;
    default:
        break;
    }

    return buf;
}

void write_push(FILE *o_stream, Segment seg, int index)
{
    fprintf(o_stream, "push %s %d\n", seg_to_str(seg), index);
}

void write_pop(FILE *o_stream, Segment seg, int index)
{
    fprintf(o_stream, "pop %s %d\n", seg_to_str(seg), index);
}

void write_arithmetic(FILE *o_stream, Operator op)
{
    fprintf(o_stream, "%s\n", op_to_str(op));
}

void write_label(FILE *o_stream, char *label_name)
{
    fprintf(o_stream, "label %s\n", label_name);
}

void write_goto(FILE *o_stream, char *label_name)
{
    fprintf(o_stream, "goto %s\n", label_name);
}

void write_if(FILE *o_stream, char *label_name)
{
    fprintf(o_stream, "if-goto %s\n", label_name);
}

void write_call(FILE *o_stream, char *name, int argc)
{
    fprintf(o_stream, "call %s %d\n", name, argc);
}

void write_function(FILE *o_stream, char *name, int argc)
{
    fprintf(o_stream, "function %s %d\n", name, argc);
}

void write_return(FILE *o_stream)
{
    fprintf(o_stream, "return\n");
}
