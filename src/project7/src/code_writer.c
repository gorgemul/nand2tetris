#include "code_writer.h"
#include <string.h>

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


void copy_segment_value_to_D_register(FILE *file, Statement *s, const char *vm_file_root)
{
    if (strcmp(s->arg1, "constant") == 0) {
        fprintf(file, "\t@%d\n", s->arg2);
        fprintf(file, "\tD=A\n");
        return;
    }

    if (strcmp(s->arg1, "static") == 0) {
        fprintf(file, "\t@%s.%d\n", vm_file_root, s->arg2);
        fprintf(file, "\tD=M\n");
        return;
    }

    if (strcmp(s->arg1, "pointer") == 0) {
        fprintf(file, "\t@%s\n", (s->arg2 == 0) ? "THIS" : "THAT");
        fprintf(file, "\tD=M\n");
        return;
    }

    if (strcmp(s->arg1, "temp") == 0) {
        fprintf(file, "\t@R%d\n", (5 + s->arg2));
        fprintf(file, "\tD=M\n");
        return;
    }

    if (strcmp(s->arg1, "local") == 0) {
        fprintf(file, "\t@LCL\n");
    } else if (strcmp(s->arg1, "argument") == 0) {
        fprintf(file, "\t@ARG\n");
    } else if (strcmp(s->arg1, "this") == 0) {
        fprintf(file, "\t@THIS\n");
    } else if (strcmp(s->arg1, "that") == 0) {
        fprintf(file, "\t@THAT\n");
    }

    fprintf(file, "\tD=M\n");
    fprintf(file, "\t@%d\n", s->arg2);
    fprintf(file, "\tA=D+A\n");
    fprintf(file, "\tD=M\n");
}

void code_writer(FILE *file, Statement *s, const char *vm_file_root)
{
    switch (s->cmd) {
    case C_ARITHMETIC:
        write_comment(file, s);
        pop_stack_value_to_D_register(file);
        copy_D_register_value_to(file, TEMP_REGISTER);
        pop_stack_value_to_D_register(file);
        add_D_register_value_with(file, TEMP_REGISTER);
        push_D_register_value_to_stack(file);
        break;
    case C_PUSH:
        write_comment(file, s);
        copy_segment_value_to_D_register(file, s, vm_file_root);
        push_D_register_value_to_stack(file);
        break;
    default:
        break;
    }
}
