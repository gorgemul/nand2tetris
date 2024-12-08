#include "code_writer.h"
#include "type.h"
#include <string.h>
#include <stdlib.h>
#include <unity.h>
#include <unity_fixture.h>

#define LINE_MAX_LENGTH 256

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

void test_push_D_register_value_to_stack(FILE *file)
{
    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\t@SP\n", line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tM=D\n", line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\t@SP\n", line);

    fgets(line, LINE_MAX_LENGTH, file);
    TEST_ASSERT_EQUAL_STRING("\tM=M+1\n", line);
}

TEST_GROUP(code_writer);

TEST(code_writer, code_writer_arithmetic_add_statement)
{
    FILE *tmp_file = tmpfile();
    Statement s = {
        .cmd = C_ARITHMETIC,
        .arg1 = "add",
        .arg2 = NO_ARG2,
    };

    code_writer(tmp_file, &s);
    rewind(tmp_file);

    test_write_comment(tmp_file, &s);
    test_pop_stack_value_to_D_register(tmp_file);
    test_copy_D_register_value_to(tmp_file, "R13");
    test_pop_stack_value_to_D_register(tmp_file);
    test_add_D_register_value_with(tmp_file, "R13");
    test_push_D_register_value_to_stack(tmp_file);

    fclose(tmp_file);
}
