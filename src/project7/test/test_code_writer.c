#include "code_writer.h"
#include "type.h"
#include <unity.h>
#include <unity_fixture.h>

TEST_SETUP(code_writer)
{

}

TEST_TEAR_DOWN(code_writer)
{
}

TEST_GROUP(code_writer);

TEST(code_writer, code_writer_arithmetic_add_statement)
{
    Statement s = {
        .cmd = C_ARITHMETIC,
        .arg1 = "add",
        .arg2 = NO_ARG2,
    };


    TEST_ASSERT_EQUAL_INT(C_ARITHMETIC, s.cmd);
}
