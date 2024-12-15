#include "parser.h"
#include "type.h"
#include <unity.h>
#include <unity_fixture.h>
#include <stdlib.h>

TEST_SETUP(parser_p8)
{
}

TEST_TEAR_DOWN(parser_p8)
{
}

TEST_GROUP(parser_p8);

TEST(parser_p8, parser_label_statement)
{
    const char *line = "label LOOP\n";
    Statement *s = parser(line);

    TEST_ASSERT_EQUAL_INT(C_LABEL, s->cmd);
    TEST_ASSERT_EQUAL_STRING("(LOOP)", s->arg1);
    TEST_ASSERT_EQUAL_INT(NO_ARG2, s->arg2);

    free(s);
}

TEST(parser_p8, parser_goto_statement)
{
    const char *line = "goto FOOBAR\n";
    Statement *s = parser(line);

    TEST_ASSERT_EQUAL_INT(C_GOTO, s->cmd);
    TEST_ASSERT_EQUAL_STRING("FOOBAR", s->arg1);
    TEST_ASSERT_EQUAL_INT(NO_ARG2, s->arg2);

    free(s);
}

TEST(parser_p8, parser_if_goto_statement)
{
    const char *line = "if-goto LOWKEY\n";
    Statement *s = parser(line);

    TEST_ASSERT_EQUAL_INT(C_IF, s->cmd);
    TEST_ASSERT_EQUAL_STRING("LOWKEY", s->arg1);
    TEST_ASSERT_EQUAL_INT(NO_ARG2, s->arg2);

    free(s);
}
