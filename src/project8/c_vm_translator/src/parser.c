#include "parser.h"
#include "type.h"
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

    if (cmd == ILLEGAL_CMD) {
        fprintf(stderr, "No such command in the statement!\n");
        exit(1);
    }

clean:
    free(token);

    return cmd;
}

void parse_one_token_statement(Statement *s, const char *content, Command cmd)
{
    s->cmd = cmd;
    strcpy(s->arg1, content);
    s->arg2 = NO_ARG2;
}

void parse_two_tokens_statement(Statement *s, const char *content, Command cmd)
{
    char *from = strchr(content, ' ') + 1;
    char *to = strchr(from, '\0');
    int length = to - from;

    char *buf = malloc(sizeof(*buf) * length+3); // "()\0"

    if (cmd == C_LABEL) strcat(buf, "(");

    strcat(buf, from);

    if (cmd == C_LABEL) strcat(buf, ")\0");

    s->cmd = cmd;
    strcpy(s->arg1, buf);
    s->arg2 = NO_ARG2;

    free(buf);
}

void parse_three_tokens_statement(Statement *s, const char *content, Command cmd)
{
    char *from = strchr(content, ' ') + 1;
    char *to = strchr(from, ' ');

    int seg_len = to - from;

    char *seg_buf = malloc(sizeof(*seg_buf) * (seg_len+1));
    memcpy(seg_buf, from, seg_len);
    seg_buf[seg_len] = '\0';

    from = to + 1;
    to = strchr(from, '\0');

    int offset_len = to - from;

    char *offset_buf = malloc(sizeof(*offset_buf) * (offset_len+1));
    memcpy(offset_buf, from, offset_len);
    offset_buf[offset_len] = '\0';

    char *end_ptr = NULL;
    int offset = strtol(offset_buf, &end_ptr, 10);

    if (end_ptr == offset_buf) {
        fprintf(stderr, "Illegal offset!\n");
        exit(1);
    }

    s->cmd = cmd;
    strcpy(s->arg1, seg_buf);
    s->arg2 = offset;

    free(seg_buf);
    free(offset_buf);
}

Statement *parser(const char *line)
{
    char *content = extract_line_content(line);
    if (!content) return NULL;

    Statement *s = malloc(sizeof(*s));
    Command cmd = get_cmd(content);

    switch (cmd) {
    case C_ARITHMETIC:
        parse_one_token_statement(s, content, C_ARITHMETIC);
        break;
    case C_PUSH:
        parse_three_tokens_statement(s, content, C_PUSH);
        break;
    case C_POP:
        parse_three_tokens_statement(s, content, C_POP);
        break;
    case C_FUNCTION:
        parse_three_tokens_statement(s, content, C_FUNCTION);
        break;
    case C_CALL:
        parse_three_tokens_statement(s, content, C_CALL);
        break;
    case C_RETURN:
        parse_one_token_statement(s, content, C_RETURN);
        break;
    case C_LABEL:
        parse_two_tokens_statement(s, content, C_LABEL);
        break;
    case C_GOTO:
        parse_two_tokens_statement(s, content, C_GOTO);
        break;
    case C_IF:
        parse_two_tokens_statement(s, content, C_IF);
        break;
    default:
        break;
    }

    free(content);
    return s;
}
