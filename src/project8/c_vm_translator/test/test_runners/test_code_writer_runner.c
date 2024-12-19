#include <unity.h>
#include <unity_fixture.h>

TEST_GROUP_RUNNER(code_writer)
{
    RUN_TEST_CASE(code_writer, code_writer_ARITHMETIC_add_statement);
    RUN_TEST_CASE(code_writer, code_writer_ARITHMETIC_sub_statement);
    RUN_TEST_CASE(code_writer, code_writer_ARITHMETIC_neg_statement);
    RUN_TEST_CASE(code_writer, code_writer_ARITHMETIC_eq_statement);
    RUN_TEST_CASE(code_writer, code_writer_ARITHMETIC_gt_statement);
    RUN_TEST_CASE(code_writer, code_writer_ARITHMETIC_lt_statement);
    RUN_TEST_CASE(code_writer, code_writer_ARITHMETIC_and_statement);
    RUN_TEST_CASE(code_writer, code_writer_ARITHMETIC_or_statement);
    RUN_TEST_CASE(code_writer, code_writer_ARITHMETIC_not_statement);

    RUN_TEST_CASE(code_writer, code_writer_BRANCHING_label_statement);
    RUN_TEST_CASE(code_writer, code_writer_BRANCHING_goto_statement);
    RUN_TEST_CASE(code_writer, code_writer_BRANCHING_if_goto_statement);

    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_push_offset_segment_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_push_constant_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_push_pointer_0_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_push_pointer_1_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_push_static_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_push_temp_statement);

    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_pop_offset_segment_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_pop_static_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_pop_pointer_0_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_pop_pointer_1_statement);
    RUN_TEST_CASE(code_writer, code_writer_MEMORY_ACCESS_pop_temp_statement);

    RUN_TEST_CASE(code_writer, code_writer_FUNCTION_definition);
    RUN_TEST_CASE(code_writer, code_writer_FUNCTION_return);
    RUN_TEST_CASE(code_writer, code_writer_FUNCTION_call);
}
