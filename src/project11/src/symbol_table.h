#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "type.h"
#include <stdio.h>

SymbolTable *init_symbol_table(FILE *file);
void destruct_symbol_table(SymbolTable *st);
void destruct_subroutine_vars(SymbolTable *st);

void add_var(SymbolTable *st, char *name, char *type, VarKind kind);
int get_var_count(SymbolTable *st, VarKind kind);

VarKind get_var_kind(SymbolTable *st, char *name);
char *get_var_type(SymbolTable *st, char *name);
int get_var_index(SymbolTable *st, char *name);

#endif
