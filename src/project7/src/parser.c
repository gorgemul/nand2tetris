#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *extract_line_content(const char *line)
{
    // trim preceiding whitespaces
    const char *begin = line;
    while (begin[0] == ' ' || begin[0] == '\t') begin++;

    int no_content = begin[0] == '\n' || begin[0] == '/';
    if (no_content) return NULL;

    char *comment = strchr(begin, '/');

    const char *end = comment ? comment : strchr(begin, '\n');

    // trim trailing whitespaces
    do {
        end--;
    } while (end[0] == ' ' || end[0] == '\t' || end[0] == '/');

    int content_length = end - begin + 1;

    char *buf = malloc(sizeof(*buf) * content_length+1);
    memcpy(buf, begin, content_length);
    buf[content_length] = '\0';

    return buf;
}

char *extract_first_token(const char *content)
{
    const char *from = content;

    char *space = strchr(from, ' ');
    char *to = space ? space : strchr(content, '\0');

    int token_len = to - from;

    char *buf = malloc(sizeof(*buf) * (token_len+1));
    memcpy(buf, from, token_len);
    buf[token_len] = '\0';

    return buf;
}

Command get_cmd(const char *content)
{
    char *token = extract_first_token(content);
    Command cmd = ILLEGAL_CMD;

    if (strcmp(token, "add") == 0 ||
        strcmp(token, "sub") == 0 ||
        strcmp(token, "neg") == 0 ||
        strcmp(token, "and") == 0 ||
        strcmp(token, "not") == 0 ||
        strcmp(token, "eq")  == 0 ||
        strcmp(token, "gt")  == 0 ||
        strcmp(token, "lt")  == 0 ||
        strcmp(token, "or")  == 0
    ) {
        cmd = C_ARITHMETIC;
        goto clean;
    }

    if (strcmp(token, "pop") == 0) {
        cmd = C_POP;
        goto clean;
    }

    if (strcmp(token, "push") == 0) {
        cmd = C_PUSH;
        goto clean;
    }

    if (strcmp(token, "label") == 0) {
        cmd = C_LABEL;
        goto clean;
    }

    if (strcmp(token, "goto") == 0) {
        cmd = C_GOTO;
        goto clean;
    }

    if (strcmp(token, "if-goto") == 0) {
        cmd = C_IF;
        goto clean;
    }

    if (strcmp(token, "function") == 0) {
        cmd = C_FUNCTION;
        goto clean;
    }

    if (strcmp(token, "call") == 0) {
        cmd = C_CALL;
        goto clean;
    }

    if (strcmp(token, "return") == 0) {
        cmd = C_RETURN;
        goto clean;
    }

    if (cmd == ILLEGAL_CMD) exit(1);

clean:
    free(token);
    return cmd;
}

Statement *parser(const char *line)
{
    char *content = extract_line_content(line);
    if (!content) return NULL;

    Statement *s = malloc(sizeof(*s));
    Command cmd = get_cmd(content);

    switch (cmd) {
    case C_ARITHMETIC:
        s->cmd = C_ARITHMETIC;
        strcpy(s->arg1, content);
        s->arg2 = NO_ARG2;
        break;
    case C_PUSH:
        /* handle_push(s, content); */
        break;
    default:
        break;
    }

    free(content);
    return s;
}
