#ifndef VM_WRITER_H
#define VM_WRITER_H

#include "type.h"
#include <stdio.h>

void write_push(FILE *o_stream, Segment seg, int index);
void write_pop(FILE *o_stream, Segment seg, int index);
void write_arithmetic(FILE *o_stream, Operator op);
void write_label(FILE *o_stream, char *label_name);
void write_goto(FILE *o_stream, char *label_name);
void write_if(FILE *o_stream, char *label_name);
void write_call(FILE *o_stream, char *name, int argc);
void write_function(FILE *o_stream, char *name, int argc);
void write_return(FILE *o_stream);

#endif
