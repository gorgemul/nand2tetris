#include "compile_engine.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

int is_dir(char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

char *get_suffix(char *file_name)
{
    char *dot = strchr(file_name, '.');
    if (dot == NULL) return NULL;
    return dot;
}

char *resolve_path(char *dir_name, char *base_name)
{
    int is_dir_end_with_slash = dir_name[strlen(dir_name)-1] == '/';

    char *buf = malloc(sizeof(*buf) * 128);

    strcat(buf, dir_name);
    if (!is_dir_end_with_slash) strcat(buf, "/");
    strcat(buf, base_name);

    return buf;
}

char *get_o_stream_path(char *i_stream_path)
{
    char *buf = malloc(sizeof(*buf) * 128);

    int no_suffix_len = strlen(i_stream_path) - 5; // ".jack"
    memcpy(buf, i_stream_path, no_suffix_len);
    buf[no_suffix_len] = '\0';

    strcat(buf, "Test.xml");

    return buf;
}

void compile_file(char *i_stream_path)
{
    char *o_stream_path = get_o_stream_path(i_stream_path);

    FILE *i_stream = fopen(i_stream_path, "r");
    FILE *o_stream = fopen(o_stream_path, "w");

    compile_class(i_stream, o_stream);

    printf("======> Success compile %s\n", i_stream_path);
    printf("======> Generating %s\n", o_stream_path);

    free(o_stream_path);
    fclose(i_stream);
    fclose(o_stream);
}

void compile_dir(char *dir_path)
{
    DIR *dir = opendir(dir_path);
    struct dirent *entry = NULL;

    printf("======> Start compile directory: %s\n", dir_path);

    while ((entry = readdir(dir))) {
        if (entry->d_type != DT_REG) continue;

        char *suffix = get_suffix(entry->d_name);
        if (!suffix || strcmp(suffix, ".jack") != 0) continue;

        char *i_stream_path = resolve_path(dir_path, entry->d_name);

        compile_file(i_stream_path);

        free(i_stream_path);
    }

    printf("======> End compile directory: %s\n", dir_path);

    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "USAGE: <Parser> <FILE | DIR>");
        return 1;
    }

    char *i_stream_path = argv[1];

    if (is_dir(i_stream_path)) compile_dir(i_stream_path);
    else                       compile_file(i_stream_path);

    return 0;
}
