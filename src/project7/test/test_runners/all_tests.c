#include <unity_fixture.h>

static void RunAllTests(void)
{
    RUN_TEST_GROUP(parser);
    RUN_TEST_GROUP(code_writer);
}

int main(int argc, const char *argv[])
{
    return UnityMain(argc, argv, RunAllTests);
}
