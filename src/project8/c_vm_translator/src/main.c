#include "parser.h"
#include "code_writer.h"
#include "type.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#define VM_ENTRY_POINT "Sys.vm"
#define BASENAME_MAX_LENGTH 256
#define LINE_MAX_LENGTH     256

char *get_file_dst_path(char *src_path)
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

char *get_dir_dst_path(char* src_path)
{
    char *buf = malloc(sizeof(*buf) * 256);

    char *from = src_path;
    while (from[0] == '.' || from[0] == '/') from++;

    int is_cur_dir = from[0] == '\0';

    if (is_cur_dir) {
        getcwd(buf, 256);
        char *curr_slash = buf;
        char *next_slash = NULL;

        while ((next_slash = strchr(curr_slash+1, '/')) != NULL) {
            curr_slash = next_slash;
        }

        strcpy(buf, curr_slash+1);
        strcat(buf, ".asm");

        return buf;
    } else {
        char *end = strchr(from, '\0');
        do { --end; } while (end[0] == '/');

        int content_length = end - from + 1;

        memcpy(buf, from, content_length);
        buf[content_length] = '\0';
        strcat(buf, ".asm");

        return buf;
    }
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

char *extract_dir_name(char* dir_path)
{
    char *from = dir_path;
    while (from[0] == '.' || from[0] == '/') from++;

    char *end = strchr(from, '\0');
    do {
        --end;
    } while (end[0] == '/');

    int content_length = end - from + 1;
    char *buf = malloc(sizeof(*buf) * content_length+1);
    memcpy(buf, from, content_length);
    buf[content_length] = '\0';

    return buf;
}

int check_if_dir(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

int check_if_entry_point(DIR *dir)
{
    struct dirent *entry = NULL;

    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, VM_ENTRY_POINT) == 0) return 1;
    }

    return 0;
}

char *get_file_path(const char *dir_path, const char *file_name)
{
    char *end = strchr(dir_path, '\0');
    int end_with_slash = *(end - 1) == '/';

    char *buf = malloc(sizeof(*buf) * 128);

    strcat(buf, dir_path);
    if (!end_with_slash) strcat(buf, "/");
    strcat(buf, file_name);

    return buf;
}

void translate_file(char *src_path)
{
    char line[LINE_MAX_LENGTH] = {0};
    char *dst_path = get_file_dst_path(src_path);
    char *dst_file_root = extract_file_root(dst_path);
    FILE *src = fopen(src_path, "r");
    FILE *dst = fopen(dst_path, "w");

    while (fgets(line, sizeof(line), src)) {
        Statement *s = parser(line);
        if (!s) continue;
        code_writer(dst, s, dst_file_root);
    }

    printf("======> Success translate %s\n", src_path);
    printf("======> Generating ./%s\n", dst_path);

    free(dst_path);
    free(dst_file_root);
    fclose(src);
    fclose(dst);
}

void init(FILE *dst)
{
    Statement s = {
        .cmd = C_CALL,
        .arg1 = "Sys.init",
        .arg2 = 0,
    };

    fprintf(dst, "\t@256\n");
    fprintf(dst, "\tD=A\n");
    fprintf(dst, "\t@SP\n");
    fprintf(dst, "\tM=D\n");
    code_writer(dst, &s, NULL);
}

char *get_suffix(const char *file_name)
{
    char *dot = strchr(file_name, '.');
    if (dot == NULL) return NULL;
    return dot+1;
}

void translate_entry_point(FILE *dst, const char *dir_path, char *dst_file_root)
{
    char line[LINE_MAX_LENGTH] = {0};
    char *src_path = get_file_path(dir_path, VM_ENTRY_POINT);
    FILE *src = fopen(src_path, "r");

    init(dst);

    while (fgets(line, sizeof(line), src)) {
        Statement *s = parser(line);
        if (!s) continue;
        code_writer(dst, s, dst_file_root);
    }

    printf("======> Success translate %s\n", src_path);

    free(src_path);
    fclose(src);
}

void translate_others(DIR *dir, FILE *dst, const char *dir_path, char *dst_file_root)
{
    rewinddir(dir);

    char line[LINE_MAX_LENGTH] = {0};
    struct dirent *entry = NULL;

    while ((entry = readdir(dir))) {
        if (entry->d_type != DT_REG || strcmp(entry->d_name, VM_ENTRY_POINT) == 0) continue;

        char *suffix = get_suffix(entry->d_name);
        if (!suffix || strcmp(suffix, "vm") != 0) continue;

        char *src_path = get_file_path(dir_path, entry->d_name);
        FILE *src = fopen(src_path, "r");
        char *src_root = extract_file_root(entry->d_name);

        while (fgets(line, sizeof(line), src)) {
            Statement *s = parser(line);
            if (!s) continue;
            code_writer(dst, s, src_root);
        }

        printf("======> Success translate %s\n", src_path);

        free(src_path);
        free(src_root);
        fclose(src);
    }

    printf("======> Generating ./%s", dst_file_root);
    printf(".asm\n");
}

void translate_dir(char *dir_path)
{
    DIR *dir = opendir(dir_path);
    int has_entry_point = check_if_entry_point(dir);

    if (!has_entry_point) {
        fprintf(stderr, "Could not find the vm entry point in providing dir!");
        exit(1);
    }

    char *dst_path = get_dir_dst_path(dir_path);
    FILE *dst = fopen(dst_path, "w");
    char *dst_file_root = extract_file_root(dst_path);
    translate_entry_point(dst, dir_path, dst_file_root);
    translate_others(dir, dst, dir_path, dst_file_root);

    free(dst_path);
    fclose(dst);
    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "USAGE: <program> <VM_FILE>");
        exit(1);
    }

    char *src_path = argv[1];
    int is_dir = check_if_dir(src_path);

    if (is_dir) translate_dir(src_path);
    else translate_file(src_path);

    return 0;
}
