#include "parser.h"
#include <unity.h>
#include <unity_fixture.h>
#include <stdlib.h>

TEST_SETUP(parser)
{

}

TEST_TEAR_DOWN(parser)
{
}

TEST_GROUP(parser);

TEST(parser, parser_comment_return_null_statement_no_whitespaces)
{
    const char *line = "// This is a comment";
    Statement *s = parser(line);
    TEST_ASSERT_NULL(s);
}

TEST(parser, parser_comment_return_null_statement_preceiding_whitespaces)
{
    const char *line = "    // This is a comment";
    Statement *s = parser(line);
    TEST_ASSERT_NULL(s);
}

TEST(parser, parser_comment_return_null_statement_trailing_whitespaces)
{
    const char *line = "// This is a comment   \n";
    Statement *s = parser(line);
    TEST_ASSERT_NULL(s);
}

TEST(parser, parser_comment_return_null_statement_preceiding_trailing_whitespaces)
{
    const char *line = "  // This is a comment   \n";
    Statement *s = parser(line);
    TEST_ASSERT_NULL(s);
}

TEST(parser, parser_empty_line_return_null_statement)
{
    const char *line = "\n";
    Statement *s = parser(line);
    TEST_ASSERT_NULL(s);
}

TEST(parser, parser_empty_line_return_null_statement_with_space)
{
    const char *line = " \n";
    Statement *s = parser(line);
    TEST_ASSERT_NULL(s);
}

TEST(parser, parser_empty_line_return_null_statement_with_tab)
{
    const char *line = "\t\n";
    Statement *s = parser(line);
    TEST_ASSERT_NULL(s);
}

TEST(parser, parser_empty_line_return_null_statement_with_space_and_tab)
{
    const char *line = " \t\n";
    Statement *s = parser(line);
    TEST_ASSERT_NULL(s);
}

TEST(parser, parser_arithmetic_statement)
{
    const char *line = "add\n";
    Statement *s = parser(line);

    TEST_ASSERT_EQUAL_INT(C_ARITHMETIC, s->cmd);
    TEST_ASSERT_EQUAL_STRING("add", s->arg1);
    TEST_ASSERT_EQUAL_INT(NO_ARG2, s->arg2);

    free(s);
}

TEST(parser, parser_arithmetic_statement_with_preceiding_whitespaces)
{
    const char *line = "  gt\n";
    Statement *s = parser(line);

    TEST_ASSERT_EQUAL_INT(C_ARITHMETIC, s->cmd);
    TEST_ASSERT_EQUAL_STRING("gt", s->arg1);
    TEST_ASSERT_EQUAL_INT(NO_ARG2, s->arg2);

    free(s);
}

TEST(parser, parser_arithmetic_statement_with_trailing_whitespaces)
{
    const char *line = "sub      \n";
    Statement *s = parser(line);

    TEST_ASSERT_EQUAL_INT(C_ARITHMETIC, s->cmd);
    TEST_ASSERT_EQUAL_STRING("sub", s->arg1);
    TEST_ASSERT_EQUAL_INT(NO_ARG2, s->arg2);

    free(s);
}

TEST(parser, parser_arithmetic_statement_with_trailing_with_comment)
{
    const char *line = "sub //some comment \n";
    Statement *s = parser(line);

    TEST_ASSERT_EQUAL_INT(C_ARITHMETIC, s->cmd);
    TEST_ASSERT_EQUAL_STRING("sub", s->arg1);
    TEST_ASSERT_EQUAL_INT(NO_ARG2, s->arg2);

    free(s);
}

TEST(parser, parser_push_statement)
{
    const char *line = "push local 0\n";
    Statement *s = parser(line);

    TEST_ASSERT_EQUAL_INT(C_PUSH, s->cmd);
    TEST_ASSERT_EQUAL_STRING("local", s->arg1);
    TEST_ASSERT_EQUAL_INT(0, s->arg2);

    free(s);
}
