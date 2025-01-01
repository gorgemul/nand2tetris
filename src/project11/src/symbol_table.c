#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

VarKind get_kind(char *var)
{
    switch (var[0]) {
    case 'S':
        return STATIC;
    case 'F':
        return FIELD;
    case 'A':
        return ARG;
    case 'V':
        return VAR;
    default:
        fprintf(stderr, "No such variable kind");
        exit(1);
    }
}

char *get_type(char *var)
{
    char *buf = malloc(sizeof(*buf) * MAX_VAR_NAME);
    char *from = strchr(var, '_') + 1;
    char *to = strchr(from, '_');

    int type_len = to - from;
    memcpy(buf, from, type_len);
    buf[type_len] = '\0';

    return buf;
}

char *get_name(char *var)
{
    char *from = var;
    char *to = NULL;
    while ((to = strchr(from, '_'))) from = to + 1;

    return from;
}

char *get_class_name(char *filePath)
{
    char *buf = malloc(sizeof(*buf) * 64);
    char *from = filePath;
    char *to = NULL;

    while ((to = strchr(from, '/'))) from = to + 1;

    char *dot = strchr(from, '.');
    int len = dot - from;
    memcpy(buf, from, len);
    buf[len] = '\0';

    return buf;
}

SymbolTable *init_symbol_table(FILE *file)
{
    SymbolTable *st = malloc(sizeof(*st));

    st->class_var_count = 0;
    st->subroutine_var_count = 0;

    int fd = fileno(file);
    char filePath[0xFFF] = {0};
    fcntl(fd, F_GETPATH, filePath);

    char *class_name = get_class_name(filePath);
    strcpy(st->class_name, class_name);

    st->class_vars = NULL;
    st->subroutine_vars = NULL;

    free(class_name);

    return st;
}

void destruct_subroutine_vars(SymbolTable *st)
{
    if (st->subroutine_var_count == 0) return;
    for (int i = 0; i < st->subroutine_var_count; i++) free(st->subroutine_vars[i]);
    free(st->subroutine_vars);

    st->subroutine_var_count = 0;
}

void destruct_symbol_table(SymbolTable *st)
{
    if (st->class_var_count > 0) {
        for (int i = 0; i < st->class_var_count; i++) free(st->class_vars[i]);
        free(st->class_vars);
    }
    free(st);
}

// varname_type_count_dec
void add_var(SymbolTable *st, char *name, char *type, VarKind kind)
{
    char *buf = malloc(sizeof(*buf) * MAX_VAR_NAME);
    char **newAddr = NULL;

    switch (kind) {
    case STATIC:
        buf[0] = 'S';
        goto append_class_var;
    case FIELD:
        buf[0] = 'F';
append_class_var:
        newAddr = ((++st->class_var_count) == 1)
            ? malloc(sizeof(char**) * st->class_var_count)
            : realloc(st->class_vars, sizeof(char**) * st->class_var_count);
        st->class_vars = newAddr;

        buf[1] = '\0';
        strcat(buf, "_");
        strcat(buf, type);
        strcat(buf, "_");
        strcat(buf, name);

        st->class_vars[st->class_var_count-1] = buf;

        break;
    case ARG:
        buf[0] = 'A';
        goto append_subroutine_var;
    case VAR:
        buf[0] = 'V';
append_subroutine_var:
        newAddr = ((++st->subroutine_var_count) == 1)
            ? malloc(sizeof(char**) * st->subroutine_var_count)
            : realloc(st->subroutine_vars, sizeof(char**) * st->subroutine_var_count);
        st->subroutine_vars = newAddr;

        buf[1] = '\0';
        strcat(buf, "_");
        strcat(buf, type);
        strcat(buf, "_");
        strcat(buf, name);

        st->subroutine_vars[st->subroutine_var_count-1] = buf;
        break;
    default:
        fprintf(stderr, "No such variable kind");
        exit(1);
    }
}

VarKind get_var_kind(SymbolTable *st, char *name)
{
    for (int i = 0; i < st->subroutine_var_count; i++) {
        char *var = st->subroutine_vars[i];
        if (strcmp(get_name(var), name) == 0) return get_kind(var);
    }

    for (int i = 0; i < st->class_var_count; i++) {
        char *var = st->class_vars[i];
        if (strcmp(get_name(var), name) == 0) return get_kind(var);
    }

    return NOT_VAR;
}

int get_var_count(SymbolTable *st, VarKind kind)
{
    int counter = 0;

    switch (kind) {
    case STATIC:
    case FIELD:
        for (int i = 0; i < st->class_var_count; i++) {
            if (get_kind(st->class_vars[i]) == kind) counter++;
        }
        break;
    case ARG:
    case VAR:
        for (int i = 0; i < st->subroutine_var_count; i++) {
            if (get_kind(st->subroutine_vars[i]) == kind) counter++;
        }
        break;
    default:
        fprintf(stderr, "No such variable kind");
        exit(1);
    }

    return counter;
}

char *get_var_type(SymbolTable *st, char *name)
{
    char *type = NULL;
    VarKind kind = get_var_kind(st, name);
    if (kind == NOT_VAR) return NULL;

    if (kind == STATIC || kind == FIELD) {
        for (int i = 0; i < st->class_var_count; i++) {
            char *var = st->class_vars[i];

            if (strcmp(get_name(var), name) != 0) continue;

            type = get_type(var);
        }
    }

    if (kind == ARG || kind == VAR) {
        for (int i = 0; i < st->subroutine_var_count; i++) {
            char *var = st->subroutine_vars[i];

            if (strcmp(get_name(var), name) != 0) continue;

            type = get_type(var);
        }
    }

    return type;
}

int get_var_index(SymbolTable *st, char *name)
{
    VarKind kind = get_var_kind(st, name);
    if (kind == NOT_VAR) return -1;

    int index = 0;

    if (kind == STATIC || kind == FIELD) {
        for (int i = 0; i < st->class_var_count; i++) {
            char *var = st->class_vars[i];

            if (get_kind(var) != kind) continue;
            if (strcmp(get_name(var), name) == 0) break;

            index++;
        }
    } else if (kind == ARG || kind == VAR) {
        for (int i = 0; i < st->subroutine_var_count; i++) {
            char *var = st->subroutine_vars[i];

            if (get_kind(var) != kind) continue;
            if (strcmp(get_name(var), name) == 0) break;

            index++;
        }
    }

    return index;
}
