#include <unity.h>
#include <unity_fixture.h>

TEST_GROUP_RUNNER(parser_p7)
{
    RUN_TEST_CASE(parser_p7, parser_comment_return_null_statement_no_whitespaces);
    RUN_TEST_CASE(parser_p7, parser_comment_return_null_statement_preceiding_whitespaces);
    RUN_TEST_CASE(parser_p7, parser_comment_return_null_statement_trailing_whitespaces);
    RUN_TEST_CASE(parser_p7, parser_comment_return_null_statement_preceiding_trailing_whitespaces);

    RUN_TEST_CASE(parser_p7, parser_empty_line_return_null_statement_with_space);
    RUN_TEST_CASE(parser_p7, parser_empty_line_return_null_statement_with_tab);
    RUN_TEST_CASE(parser_p7, parser_empty_line_return_null_statement_with_space_and_tab);

    RUN_TEST_CASE(parser_p7, parser_arithmetic_statement);
    RUN_TEST_CASE(parser_p7, parser_arithmetic_statement_with_preceiding_whitespaces);
    RUN_TEST_CASE(parser_p7, parser_arithmetic_statement_with_trailing_whitespaces);
    RUN_TEST_CASE(parser_p7, parser_arithmetic_statement_with_trailing_with_comment);

    RUN_TEST_CASE(parser_p7, parser_push_statement);
    RUN_TEST_CASE(parser_p7, parser_pop_statement);

    RUN_TEST_CASE(parser_p7, parser_function_statement);
    RUN_TEST_CASE(parser_p7, parser_call_statement);
    RUN_TEST_CASE(parser_p7, parser_return_statement);
}
