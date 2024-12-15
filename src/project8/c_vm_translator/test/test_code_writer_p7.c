#include "code_writer.h"
#include "type.h"
#include <string.h>
#include <stdlib.h>
#include <unity.h>
#include <unity_fixture.h>

#define LINE_MAX_LENGTH 256

TEST_SETUP(code_writer_p7)
{

}

TEST_TEAR_DOWN(code_writer_p7)
{
}

/*
===============================================================================
    Helper function for test with the purpose of providing more text context
===============================================================================
*/

char line[LINE_MAX_LENGTH] = {0};

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
    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\t@SP\n", line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tAM=M-1\n", line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tD=M\n", line);
}

void test_copy_D_register_value_to(FILE *file, const char *dst)
{
    char expected[LINE_MAX_LENGTH] = {0};
    strcat(expected, "\t");
    strcat(expected, "@");
    strcat(expected, dst);
    strcat(expected, "\n\0");

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING(expected, line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tM=D\n", line);
}

void test_add_D_register_value_with(FILE *file, const char *src)
{
    char expected[LINE_MAX_LENGTH] = {0};

    strcat(expected, "\t");
    strcat(expected, "@");
    strcat(expected, src);
    strcat(expected, "\n\0");

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING(expected, line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tD=D+M\n", line);
}

void test_sub_D_register_value_with(FILE *file, const char *src)
{
    char expected[LINE_MAX_LENGTH] = {0};

    strcat(expected, "\t");
    strcat(expected, "@");
    strcat(expected, src);
    strcat(expected, "\n\0");

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING(expected, line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tD=D-M\n", line);
}

void test_and_D_register_value_with(FILE *file, const char *src)
{
    char expected[LINE_MAX_LENGTH] = {0};

    strcat(expected, "\t");
    strcat(expected, "@");
    strcat(expected, src);
    strcat(expected, "\n\0");

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING(expected, line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tD=D&M\n", line);
}

void test_or_D_register_value_with(FILE *file, const char *src)
{
    char expected[LINE_MAX_LENGTH] = {0};

    strcat(expected, "\t");
    strcat(expected, "@");
    strcat(expected, src);
    strcat(expected, "\n\0");

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING(expected, line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tD=D|M\n", line);
}

void test_push_D_register_value_to_stack(FILE *file)
{
    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\t@SP\n", line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tA=M\n", line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tM=D\n", line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\t@SP\n", line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tM=M+1\n", line);
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

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING(expected, line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tD=M\n", line);

    fgets(line, LINE_MAX_LENGTH, file);

    memset(expected, 0, sizeof(expected));
    strcat(expected, "\t");
    strcat(expected, "@");
    sprintf(arg2_str, "%d", s->arg2);
    strcat(expected, arg2_str);
    strcat(expected, "\n\0");

    TEST_ASSERT_EQUAL_STRING(expected, line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tA=D+A\n", line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tD=M\n", line);
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

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING(expected, line);

    fgets(line, LINE_MAX_LENGTH, file);

    memset(expected, 0, sizeof(expected));
    strcat(expected, "\t");
    strcat(expected, "@");
    sprintf(arg2_str, "%d", s->arg2);
    strcat(expected, arg2_str);
    strcat(expected, "\n\0");

    TEST_ASSERT_EQUAL_STRING(expected, line);
}

TEST_GROUP(code_writer_p7);

TEST(code_writer_p7, code_writer_ARITHMETIC_add_statement)
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

TEST(code_writer_p7, code_writer_ARITHMETIC_sub_statement)
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

TEST(code_writer_p7, code_writer_ARITHMETIC_neg_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=-D\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

/*
  The conditional counter is a global variable, test would affect code writer's counter state
  So the next three tests' counter would be like 1 2 3
*/
TEST(code_writer_p7, code_writer_ARITHMETIC_eq_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@EQ1\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD;JEQ\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=0\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@NEXT1\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t0;JMP\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("(EQ1)\n", line);


    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=-1\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("(NEXT1)\n", line);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_ARITHMETIC_gt_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@GT2\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD;JGT\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=0\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@NEXT2\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t0;JMP\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("(GT2)\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=-1\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("(NEXT2)\n", line);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_ARITHMETIC_lt_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@LT3\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD;JLT\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=0\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@NEXT3\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t0;JMP\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("(LT3)\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=-1\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("(NEXT3)\n", line);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_ARITHMETIC_and_statement)
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

TEST(code_writer_p7, code_writer_ARITHMETIC_or_statement)
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

TEST(code_writer_p7, code_writer_ARITHMETIC_not_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=!D\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_MEMORY_ACCESS_push_offset_segment_statement)
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

TEST(code_writer_p7, code_writer_MEMORY_ACCESS_push_constant_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@5000\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=A\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_MEMORY_ACCESS_push_pointer_0_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@THIS\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=M\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_MEMORY_ACCESS_push_pointer_1_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@THAT\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=M\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_MEMORY_ACCESS_push_static_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@Foobar.5\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=M\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_MEMORY_ACCESS_push_temp_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@R8\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=M\n", line);

    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_MEMORY_ACCESS_pop_offset_segment_statement)
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
    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@THAT\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=M\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@3321\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tD=D+A\n", line);

    test_copy_D_register_value_to(tmp_file, TEMP_REGISTER);

    test_pop_stack_value_to_D_register(tmp_file);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@R13\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tA=M\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tM=D\n", line);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_MEMORY_ACCESS_pop_static_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@Bazbaz.100\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tM=D\n", line);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_MEMORY_ACCESS_pop_pointer_0_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@THIS\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tM=D\n", line);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_MEMORY_ACCESS_pop_pointer_1_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@THAT\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tM=D\n", line);

    fclose(tmp_file);
}

TEST(code_writer_p7, code_writer_MEMORY_ACCESS_pop_temp_statement)
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

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\t@R10\n", line);

    fgets(line, LINE_MAX_LENGTH, tmp_file);
    TEST_ASSERT_EQUAL_STRING("\tM=D\n", line);

    fclose(tmp_file);
}
