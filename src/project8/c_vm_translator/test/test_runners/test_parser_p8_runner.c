#include <unity.h>
#include <unity_fixture.h>

TEST_GROUP_RUNNER(parser_p8)
{
    RUN_TEST_CASE(parser_p8, parser_label_statement);
    RUN_TEST_CASE(parser_p8, parser_goto_statement);
    RUN_TEST_CASE(parser_p8, parser_if_goto_statement);
}
