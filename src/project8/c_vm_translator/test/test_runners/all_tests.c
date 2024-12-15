#include <unity_fixture.h>

static void RunAllTests(void)
{
    RUN_TEST_GROUP(parser_p7);
    RUN_TEST_GROUP(code_writer_p7);

    RUN_TEST_GROUP(parser_p8);
}

int main(int argc, const char *argv[])
{
    return UnityMain(argc, argv, RunAllTests);
}
