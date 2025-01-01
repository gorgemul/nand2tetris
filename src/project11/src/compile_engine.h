#ifndef COMPILE_ENGINE_H
#define COMPILE_ENGINE_H

#include "type.h"
#include <stdio.h>

void compile_class(FILE *i_stream, FILE *o_stream);
void compile_class_var_dec(FILE *i_stream, FILE *o_stream);
void compile_subroutine_dec(FILE *i_stream, FILE *o_stream);
void compile_parameter_list(FILE *i_stream, FILE *o_stream);
void compile_subroutine_body(FILE *i_stream, FILE *o_stream);
void compile_var_dec(FILE *i_stream, FILE *o_stream);

void compile_statements(FILE *i_stream, FILE *o_stream);
void compile_let_statement(FILE *i_stream, FILE *o_stream);
void compile_if_statement(FILE *i_stream, FILE *o_stream);
void compile_while_statement(FILE *i_stream, FILE *o_stream);
void compile_do_statement(FILE *i_stream, FILE *o_stream);
void compile_return_statement(FILE *i_stream, FILE *o_stream);

void compile_expression(FILE *i_stream, FILE *o_stream);
void compile_term(FILE *i_stream, FILE *o_stream);
void compile_expression_list(FILE *i_stream, FILE *o_stream);

#endif
