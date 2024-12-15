#include "code_writer.h"
#include "type.h"
#include <string.h>
#include <stdlib.h>
#include <unity.h>
#include <unity_fixture.h>

#define LINE_MAX_LENGTH 256
char line[LINE_MAX_LENGTH] = {0};

TEST_SETUP(code_writer)
{

}

TEST_TEAR_DOWN(code_writer)
{
}

/*
===============================================================================
    Helper function for test with the purpose of providing more text context
===============================================================================
*/

void test_current_line_equal(FILE *file, const char* content)
{
    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING(content, line);
}

void test_write_comment(FILE *file, Statement *s)
{
    char expected[LINE_MAX_LENGTH] = {0};
    char arg2_str[10] = {0};

    fgets(line, LINE_MAX_LENGTH, file);

    strcat(expected, "// ");

    switch (s->cmd) {
    case C_ARITHMETIC:
    case C_RETURN:
        strcat(expected, s->arg1);
        strcat(expected, "\n\0");
        TEST_ASSERT_EQUAL_STRING(expected, line);
        break;
    case C_PUSH:
        sprintf(arg2_str, "%d", s->arg2);
        strcat(expected, "push");
        strcat(expected, " ");
        strcat(expected, s->arg1);
        strcat(expected, " ");
        strcat(expected, arg2_str);
        strcat(expected, "\n\0");
        TEST_ASSERT_EQUAL_STRING(expected, line);
        break;
    case C_POP:
        sprintf(arg2_str, "%d", s->arg2);
        strcat(expected, "pop");
        strcat(expected, " ");
        strcat(expected, s->arg1);
        strcat(expected, " ");
        strcat(expected, arg2_str);
        strcat(expected, "\n\0");
        TEST_ASSERT_EQUAL_STRING(expected, line);
        break;
    case C_LABEL:
        strcat(expected, "label");
        strcat(expected, " ");
        strcat(expected, s->arg1);
        strcat(expected, "\n\0");
        TEST_ASSERT_EQUAL_STRING(expected, line);
        break;
    case C_GOTO:
        strcat(expected, "goto");
        strcat(expected, " ");
        strcat(expected, s->arg1);
        strcat(expected, "\n\0");
        TEST_ASSERT_EQUAL_STRING(expected, line);
        break;
    case C_IF:
        strcat(expected, "if-goto");
        strcat(expected, " ");
        strcat(expected, s->arg1);
        strcat(expected, "\n\0");
        TEST_ASSERT_EQUAL_STRING(expected, line);
        break;
    case C_FUNCTION:
        sprintf(arg2_str, "%d", s->arg2);
        strcat(expected, "function");
        strcat(expected, " ");
        strcat(expected, s->arg1);
        strcat(expected, " ");
        strcat(expected, arg2_str);
        strcat(expected, "\n\0");
        TEST_ASSERT_EQUAL_STRING(expected, line);
        break;
    case C_CALL:
        sprintf(arg2_str, "%d", s->arg2);
        strcat(expected, "call");
        strcat(expected, " ");
        strcat(expected, s->arg1);
        strcat(expected, " ");
        strcat(expected, arg2_str);
        strcat(expected, "\n\0");
        TEST_ASSERT_EQUAL_STRING(expected, line);
        break;
    default:
        break;
    }
}

void test_pop_stack_value_to_D_register(FILE *file)
{
    test_current_line_equal(file, "\t@SP\n");
    test_current_line_equal(file, "\tAM=M-1\n");
    test_current_line_equal(file, "\tD=M\n");
}

void test_copy_D_register_value_to(FILE *file, const char *dst)
{
    char expected[LINE_MAX_LENGTH] = {0};
    strcat(expected, "\t");
    strcat(expected, "@");
    strcat(expected, dst);
    strcat(expected, "\n\0");

    test_current_line_equal(file, expected);
    test_current_line_equal(file, "\tM=D\n");
}

void test_add_D_register_value_with(FILE *file, const char *src)
{
    char expected[LINE_MAX_LENGTH] = {0};

    strcat(expected, "\t");
    strcat(expected, "@");
    strcat(expected, src);
    strcat(expected, "\n\0");

    test_current_line_equal(file, expected);
    test_current_line_equal(file, "\tD=D+M\n");
}

void test_sub_D_register_value_with(FILE *file, const char *src)
{
    char expected[LINE_MAX_LENGTH] = {0};

    strcat(expected, "\t");
    strcat(expected, "@");
    strcat(expected, src);
    strcat(expected, "\n\0");

    test_current_line_equal(file, expected);
    test_current_line_equal(file, "\tD=D-M\n");
}

void test_and_D_register_value_with(FILE *file, const char *src)
{
    char expected[LINE_MAX_LENGTH] = {0};

    strcat(expected, "\t");
    strcat(expected, "@");
    strcat(expected, src);
    strcat(expected, "\n\0");

    test_current_line_equal(file, expected);
    test_current_line_equal(file, "\tD=D&M\n");
}

void test_or_D_register_value_with(FILE *file, const char *src)
{
    char expected[LINE_MAX_LENGTH] = {0};

    strcat(expected, "\t");
    strcat(expected, "@");
    strcat(expected, src);
    strcat(expected, "\n\0");

    test_current_line_equal(file, expected);
    test_current_line_equal(file, "\tD=D|M\n");
}

void test_push_D_register_value_to_stack(FILE *file)
{
    test_current_line_equal(file, "\t@SP\n");
    test_current_line_equal(file, "\tA=M\n");
    test_current_line_equal(file, "\tM=D\n");
    test_current_line_equal(file, "\t@SP\n");
    test_current_line_equal(file, "\tM=M+1\n");
}

// local, argument, this, that
void test_copy_offset_segment_value_to_D_register(FILE *file, Statement *s)
{
    char expected[LINE_MAX_LENGTH] = {0};
    char arg2_str[10] = {0};

    strcat(expected, "\t");
    strcat(expected, "@");

    if (strcmp(s->arg1, "local") == 0) {
        strcat(expected, "LCL");
    } else if (strcmp(s->arg1, "argument") == 0) {
        strcat(expected, "ARG");
    } else if (strcmp(s->arg1, "this") == 0) {
        strcat(expected, "THIS");
    } else if (strcmp(s->arg1, "that") == 0) {
        strcat(expected, "THAT");
    }

    strcat(expected, "\n\0");

    test_current_line_equal(file, expected);
    test_current_line_equal(file, "\tD=M\n");

    memset(expected, 0, sizeof(expected));
    strcat(expected, "\t");
    strcat(expected, "@");
    sprintf(arg2_str, "%d", s->arg2);
    strcat(expected, arg2_str);
    strcat(expected, "\n\0");

    test_current_line_equal(file, expected);
    test_current_line_equal(file, "\tA=D+A\n");
    test_current_line_equal(file, "\tD=M\n");
}

void test_copy_D_register_value_to_offset_segment(FILE *file, Statement *s)
{
    char expected[LINE_MAX_LENGTH] = {0};
    char arg2_str[10] = {0};

    strcat(expected, "\t");
    strcat(expected, "@");

    if (strcmp(s->arg1, "local") == 0) {
        strcat(expected, "LCL");
    } else if (strcmp(s->arg1, "argument") == 0) {
        strcat(expected, "ARG");
    } else if (strcmp(s->arg1, "this") == 0) {
        strcat(expected, "THIS");
    } else if (strcmp(s->arg1, "that") == 0) {
        strcat(expected, "THAT");
    }

    strcat(expected, "\n\0");

    test_current_line_equal(file, expected);

    memset(expected, 0, sizeof(expected));
    strcat(expected, "\t");
    strcat(expected, "@");
    sprintf(arg2_str, "%d", s->arg2);
    strcat(expected, arg2_str);
    strcat(expected, "\n\0");

    test_current_line_equal(file, expected);
}

TEST_GROUP(code_writer);

TEST(code_writer, code_writer_ARITHMETIC_add_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_ARITHMETIC,
        .arg1 = "add",
        .arg2 = NO_ARG2,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_copy_D_register_value_to(tmp_file, TEMP_REGISTER);
    test_pop_stack_value_to_D_register(tmp_file);
    test_add_D_register_value_with(tmp_file, TEMP_REGISTER);
    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer, code_writer_ARITHMETIC_sub_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_ARITHMETIC,
        .arg1 = "sub",
        .arg2 = NO_ARG2,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_copy_D_register_value_to(tmp_file, TEMP_REGISTER);
    test_pop_stack_value_to_D_register(tmp_file);
    test_sub_D_register_value_with(tmp_file, TEMP_REGISTER);
    test_push_D_register_value_to_stack(tmp_file);
    fclose(tmp_file);
}

TEST(code_writer, code_writer_ARITHMETIC_neg_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_ARITHMETIC,
        .arg1 = "neg",
        .arg2 = NO_ARG2,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_current_line_equal(tmp_file, "\tD=-D\n");
    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

/*
  The conditional counter is a global variable, test would affect code writer's counter state
  So the next three tests' counter would be like 1 2 3
*/
TEST(code_writer, code_writer_ARITHMETIC_eq_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_ARITHMETIC,
        .arg1 = "eq",
        .arg2 = NO_ARG2,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_copy_D_register_value_to(tmp_file, TEMP_REGISTER);
    test_pop_stack_value_to_D_register(tmp_file);
    test_sub_D_register_value_with(tmp_file, TEMP_REGISTER);
    test_current_line_equal(tmp_file, "\t@EQ1\n");
    test_current_line_equal(tmp_file, "\tD;JEQ\n");
    test_current_line_equal(tmp_file, "\tD=0\n");
    test_push_D_register_value_to_stack(tmp_file);
    test_current_line_equal(tmp_file, "\t@NEXT1\n");
    test_current_line_equal(tmp_file, "\t0;JMP\n");
    test_current_line_equal(tmp_file, "(EQ1)\n");
    test_current_line_equal(tmp_file, "\tD=-1\n");
    test_push_D_register_value_to_stack(tmp_file);
    test_current_line_equal(tmp_file, "(NEXT1)\n");

    fclose(tmp_file);
}

TEST(code_writer, code_writer_ARITHMETIC_gt_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_ARITHMETIC,
        .arg1 = "gt",
        .arg2 = NO_ARG2,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_copy_D_register_value_to(tmp_file, TEMP_REGISTER);
    test_pop_stack_value_to_D_register(tmp_file);
    test_sub_D_register_value_with(tmp_file, TEMP_REGISTER);
    test_current_line_equal(tmp_file, "\t@GT2\n");
    test_current_line_equal(tmp_file, "\tD;JGT\n");
    test_current_line_equal(tmp_file, "\tD=0\n");
    test_push_D_register_value_to_stack(tmp_file);
    test_current_line_equal(tmp_file, "\t@NEXT2\n");
    test_current_line_equal(tmp_file, "\t0;JMP\n");
    test_current_line_equal(tmp_file, "(GT2)\n");
    test_current_line_equal(tmp_file, "\tD=-1\n");
    test_push_D_register_value_to_stack(tmp_file);
    test_current_line_equal(tmp_file, "(NEXT2)\n");

    fclose(tmp_file);
}

TEST(code_writer, code_writer_ARITHMETIC_lt_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_ARITHMETIC,
        .arg1 = "lt",
        .arg2 = NO_ARG2,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_copy_D_register_value_to(tmp_file, TEMP_REGISTER);
    test_pop_stack_value_to_D_register(tmp_file);
    test_sub_D_register_value_with(tmp_file, TEMP_REGISTER);
    test_current_line_equal(tmp_file, "\t@LT3\n");
    test_current_line_equal(tmp_file, "\tD;JLT\n");
    test_current_line_equal(tmp_file, "\tD=0\n");
    test_push_D_register_value_to_stack(tmp_file);
    test_current_line_equal(tmp_file, "\t@NEXT3\n");
    test_current_line_equal(tmp_file, "\t0;JMP\n");
    test_current_line_equal(tmp_file, "(LT3)\n");
    test_current_line_equal(tmp_file, "\tD=-1\n");
    test_push_D_register_value_to_stack(tmp_file);
    test_current_line_equal(tmp_file, "(NEXT3)\n");

    fclose(tmp_file);
}

TEST(code_writer, code_writer_ARITHMETIC_and_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_ARITHMETIC,
        .arg1 = "and",
        .arg2 = NO_ARG2,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_copy_D_register_value_to(tmp_file, TEMP_REGISTER);
    test_pop_stack_value_to_D_register(tmp_file);
    test_and_D_register_value_with(tmp_file, TEMP_REGISTER);
    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer, code_writer_ARITHMETIC_or_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_ARITHMETIC,
        .arg1 = "or",
        .arg2 = NO_ARG2,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_copy_D_register_value_to(tmp_file, TEMP_REGISTER);
    test_pop_stack_value_to_D_register(tmp_file);
    test_or_D_register_value_with(tmp_file, TEMP_REGISTER);
    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer, code_writer_ARITHMETIC_not_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_ARITHMETIC,
        .arg1 = "not",
        .arg2 = NO_ARG2,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_current_line_equal(tmp_file, "\tD=!D\n");
    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer, code_writer_BRANCHING_label_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_LABEL,
        .arg1 = "LOOP",
        .arg2 = NO_ARG2,
    };
    const char *vm_file_root = "Foo";
    char expected[LINE_MAX_LENGTH] = {0};

    code_writer(tmp_file, &s, vm_file_root);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);

    // (Foo_LOOP)  in case nameing colision for different files
    strcat(expected, "(");
    strcat(expected, vm_file_root);
    strcat(expected, "_");
    strcat(expected, "LOOP");
    strcat(expected, ")\n");

    test_current_line_equal(tmp_file, expected);

    fclose(tmp_file);
}

TEST(code_writer, code_writer_BRANCHING_goto_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_GOTO,
        .arg1 = "LOOP",
        .arg2 = NO_ARG2,
    };
    const char *vm_file_root = "Foo";

    code_writer(tmp_file, &s, vm_file_root);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_current_line_equal(tmp_file, "\t@Foo_LOOP\n");
    test_current_line_equal(tmp_file, "\t0;JMP\n");

    fclose(tmp_file);
}

TEST(code_writer, code_writer_BRANCHING_if_goto_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_IF,
        .arg1 = "LOOP",
        .arg2 = NO_ARG2,
    };
    const char *vm_file_root = "Foo";

    code_writer(tmp_file, &s, vm_file_root);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_current_line_equal(tmp_file, "\t@Foo_LOOP\n");
    test_current_line_equal(tmp_file, "\tD;JGT\n");

    fclose(tmp_file);
}

TEST(code_writer, code_writer_MEMORY_ACCESS_push_offset_segment_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_PUSH,
        .arg1 = "local",
        .arg2 = 2000,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_copy_offset_segment_value_to_D_register(tmp_file, &s);
    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer, code_writer_MEMORY_ACCESS_push_constant_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_PUSH,
        .arg1 = "constant",
        .arg2 = 5000,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_current_line_equal(tmp_file, "\t@5000\n");
    test_current_line_equal(tmp_file, "\tD=A\n");
    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer, code_writer_MEMORY_ACCESS_push_pointer_0_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_PUSH,
        .arg1 = "pointer",
        .arg2 = 0,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_current_line_equal(tmp_file, "\t@THIS\n");
    test_current_line_equal(tmp_file, "\tD=M\n");
    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer, code_writer_MEMORY_ACCESS_push_pointer_1_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_PUSH,
        .arg1 = "pointer",
        .arg2 = 1,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_current_line_equal(tmp_file, "\t@THAT\n");
    test_current_line_equal(tmp_file, "\tD=M\n");
    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer, code_writer_MEMORY_ACCESS_push_static_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_PUSH,
        .arg1 = "static",
        .arg2 = 5,
    };
    const char* vm_file_root = "Foobar";

    code_writer(tmp_file, &s, vm_file_root);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_current_line_equal(tmp_file, "\t@Foobar.5\n");
    test_current_line_equal(tmp_file, "\tD=M\n");
    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer, code_writer_MEMORY_ACCESS_push_temp_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_PUSH,
        .arg1 = "temp",
        .arg2 = 3,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_current_line_equal(tmp_file, "\t@R8\n");
    test_current_line_equal(tmp_file, "\tD=M\n");
    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer, code_writer_MEMORY_ACCESS_pop_offset_segment_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_POP,
        .arg1 = "that",
        .arg2 = 3321,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_current_line_equal(tmp_file, "\t@THAT\n");
    test_current_line_equal(tmp_file, "\tD=M\n");
    test_current_line_equal(tmp_file, "\t@3321\n");
    test_current_line_equal(tmp_file, "\tD=D+A\n");
    test_copy_D_register_value_to(tmp_file, TEMP_REGISTER);
    test_pop_stack_value_to_D_register(tmp_file);
    test_current_line_equal(tmp_file, "\t@R13\n");
    test_current_line_equal(tmp_file, "\tA=M\n");
    test_current_line_equal(tmp_file, "\tM=D\n");

    fclose(tmp_file);
}

TEST(code_writer, code_writer_MEMORY_ACCESS_pop_static_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_POP,
        .arg1 = "static",
        .arg2 = 100,
    };
    const char *vm_file_root = "Bazbaz";

    code_writer(tmp_file, &s, vm_file_root);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_current_line_equal(tmp_file, "\t@Bazbaz.100\n");
    test_current_line_equal(tmp_file, "\tM=D\n");

    fclose(tmp_file);
}

TEST(code_writer, code_writer_MEMORY_ACCESS_pop_pointer_0_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_POP,
        .arg1 = "pointer",
        .arg2 = 0,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_current_line_equal(tmp_file, "\t@THIS\n");
    test_current_line_equal(tmp_file, "\tM=D\n");

    fclose(tmp_file);
}

TEST(code_writer, code_writer_MEMORY_ACCESS_pop_pointer_1_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_POP,
        .arg1 = "pointer",
        .arg2 = 1,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_current_line_equal(tmp_file, "\t@THAT\n");
    test_current_line_equal(tmp_file, "\tM=D\n");

    fclose(tmp_file);
}

TEST(code_writer, code_writer_MEMORY_ACCESS_pop_temp_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_POP,
        .arg1 = "temp",
        .arg2 = 5,
    };

    code_writer(tmp_file, &s, NULL);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_current_line_equal(tmp_file, "\t@R10\n");
    test_current_line_equal(tmp_file, "\tM=D\n");

    fclose(tmp_file);
}
