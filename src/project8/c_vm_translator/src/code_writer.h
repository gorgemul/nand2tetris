#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#include "type.h"
#include <stdio.h>

void code_writer(FILE *file, Statement *s, const char *vm_file_root);

#endif
