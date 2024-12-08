#include "code_writer.h"

void write_comment(FILE *file, Statement *s)
{
    fprintf(file, "// ");

    switch (s->cmd) {
    case C_ARITHMETIC:
    case C_RETURN:
        fprintf(file, "%s\n", s->arg1);
        break;
    case C_PUSH:
        fprintf(file, "push %s %d\n", s->arg1, s->arg2);
        break;
    case C_POP:
        fprintf(file, "pop %s %d\n", s->arg1, s->arg2);
        break;
    case C_FUNCTION:
        fprintf(file, "function %s %d\n", s->arg1, s->arg2);
        break;
    case C_CALL:
        fprintf(file, "call %s %d\n", s->arg1, s->arg2);
        break;
    default:
        break;
    }
}

void pop_stack_value_to_D_register(FILE *file)
{
    fprintf(file, "\t@SP\n");
    fprintf(file, "\tAM=M-1\n");
    fprintf(file, "\tD=M\n");
}

void copy_D_register_value_to(FILE *file, const char* dst)
{
    fprintf(file, "\t@%s\n", dst);
    fprintf(file, "\tM=D\n");
}

void add_D_register_value_with(FILE *file, const char* src)
{
    fprintf(file, "\t@%s\n", src);
    fprintf(file, "\tD=D+M\n");
}

void push_D_register_value_to_stack(FILE *file)
{
    fprintf(file, "\t@SP\n");
    fprintf(file, "\tM=D\n");

    fprintf(file, "\t@SP\n");
    fprintf(file, "\tM=M+1\n");
}

void code_writer(FILE *file, Statement *s)
{
    switch (s->cmd) {
    case C_ARITHMETIC:
        write_comment(file, s);
        pop_stack_value_to_D_register(file);
        copy_D_register_value_to(file, "R13");
        pop_stack_value_to_D_register(file);
        add_D_register_value_with(file, "R13");
        push_D_register_value_to_stack(file);
        break;
    default:
        break;
    }
}
