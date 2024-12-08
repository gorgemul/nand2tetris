#include <unity.h>
#include <unity_fixture.h>

TEST_GROUP_RUNNER(parser)
{
    RUN_TEST_CASE(parser, parser_comment_return_null_statement_no_whitespaces);
    RUN_TEST_CASE(parser, parser_comment_return_null_statement_preceiding_whitespaces);
    RUN_TEST_CASE(parser, parser_comment_return_null_statement_trailing_whitespaces);
    RUN_TEST_CASE(parser, parser_comment_return_null_statement_preceiding_trailing_whitespaces);

    RUN_TEST_CASE(parser, parser_empty_line_return_null_statement_with_space);
    RUN_TEST_CASE(parser, parser_empty_line_return_null_statement_with_tab);
    RUN_TEST_CASE(parser, parser_empty_line_return_null_statement_with_space_and_tab);

    RUN_TEST_CASE(parser, parser_arithmetic_statement);
    RUN_TEST_CASE(parser, parser_arithmetic_statement_with_preceiding_whitespaces);
    RUN_TEST_CASE(parser, parser_arithmetic_statement_with_trailing_whitespaces);
    RUN_TEST_CASE(parser, parser_arithmetic_statement_with_trailing_with_comment);

    RUN_TEST_CASE(parser, parser_push_statement);
    RUN_TEST_CASE(parser, parser_pop_statement);

    RUN_TEST_CASE(parser, parser_function_statement);
    RUN_TEST_CASE(parser, parser_call_statement);
    RUN_TEST_CASE(parser, parser_return_statement);
}
