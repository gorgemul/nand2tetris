#include <unity.h>
#include <unity_fixture.h>

TEST_GROUP_RUNNER(code_writer)
{
    RUN_TEST_CASE(code_writer, code_writer_ARITHMETIC_add_statement);

    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_push_offset_segment_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_push_constant_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_push_pointer_0_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_push_pointer_1_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_push_static_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_push_temp_statement);
}
