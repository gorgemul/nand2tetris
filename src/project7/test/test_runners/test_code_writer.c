#include <unity.h>
#include <unity_fixture.h>

TEST_GROUP_RUNNER(code_writer)
{
    RUN_TEST_CASE(code_writer, code_writer_arithmetic_add_statement);
}
