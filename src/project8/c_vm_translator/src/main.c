#include "parser.h"
#include "code_writer.h"
#include "type.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BASENAME_MAX_LENGTH 256
#define LINE_MAX_LENGTH     256

char *get_dst_path(char *src_path)
{
    char *from = src_path;
    char *to = NULL;

    while ((to = strchr(from, '/')) != NULL) from = to + 1;

    if ((to = strchr(from, '.')) == NULL) return NULL;

    char *buf = malloc(sizeof(*buf) * BASENAME_MAX_LENGTH);
    int length = to - from;

    memcpy(buf, from, length);
    buf[length] = '\0';
    strcat(buf, ".asm");

    return buf;
}

char *extract_file_root(char *file_name)
{
    char *from = file_name;
    char *to = strchr(from, '.');

    int length = to - from;
    char *buf = malloc(sizeof(*buf) * length+1);

    memcpy(buf, from, length);
    buf[length] = '\0';

    return buf;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "USAGE: <program> <VM_FILE>");
        exit(1);
    }

    char line[LINE_MAX_LENGTH] = {0};
    char *src_path = argv[1];
    char *dst_path = get_dst_path(src_path);
    char *vm_file_root = extract_file_root(dst_path);
    FILE *src = fopen(src_path, "r");
    FILE *dst = fopen(dst_path, "w");

    while (fgets(line, sizeof(line), src) != NULL) {
        Statement *s = parser(line);
        if (!s) continue;
        code_writer(dst, s, vm_file_root);
    }

    // TODO: should add a infinite loop!

    printf("======> Success translate %s\n", src_path);
    printf("======> Generating %s\n", dst_path);

    free(dst_path);
    free(vm_file_root);
    fclose(src);
    fclose(dst);
    return 0;
}
