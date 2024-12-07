#include "code_writer.h"
#include <unity.h>
#include <unity_fixture.h>

TEST_SETUP(sub)
{

}

TEST_TEAR_DOWN(sub)
{
}

TEST_GROUP(sub);

TEST(sub, sub_ShouldReturn1)
{
    TEST_ASSERT_EQUAL(1, sub(3, 2));
}
