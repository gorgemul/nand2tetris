#include "tokenizer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void skip_to_line_after_comment_end(FILE *stream)
{
    char line[LINE_MAX_LENGTH] = {0};

    while (fgets(line, sizeof(line), stream)) {
        char *comment_symbol = strchr(line, '/');
        if (comment_symbol) break;
    }
}

int is_keyword(char *word)
{
    static const char *keywords[] = {
        "do",
        "class",
        "constructor",
        "function",
        "method",
        "field",
        "static",
        "var",
        "int",
        "char",
        "boolean",
        "void",
        "true",
        "false",
        "null",
        "this",
        "let",
        "if",
        "else",
        "while",
        "return",
        NULL,
    };

    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(keywords[i], word) == 0) return 1;
    }

    return 0;
}

int is_symbol(char c)
{
    switch (c) {
    case '{':
    case '}':
    case '(':
    case ')':
    case '[':
    case ']':
    case '.':
    case ',':
    case ';':
    case '+':
    case '-':
    case '*':
    case '/':
    case '&':
    case '|':
    case '<':
    case '>':
    case '=':
    case '~':
        return 1;
    default:
        return 0;
    }
}

int is_string_const(FILE *stream)
{
    char c = fgetc(stream);
    ungetc(c, stream);
    return c == '"';
}

int is_int_const(char *word)
{
    char *end_ptr = NULL;
    strtol(word, &end_ptr, 10);
    return *end_ptr == '\0';
}

/**
 * if no symbol in word, return -1;
 * else return the index of first symbol was found
*/
int get_first_symbol_index(char *word)
{
    int word_len = strlen(word);

    for (int i = 0; i < word_len; i++) {
        if (is_symbol(word[i])) return i;
    }

    return -1;
}

char *get_word(FILE *stream)
{
    char line[LINE_MAX_LENGTH] = {0};

    fgets(line, sizeof(line), stream);

    char *end = strpbrk(line, " \n");
    int token_len = end - line;

    char *buf = malloc(sizeof(*buf) * token_len+1);
    memcpy(buf, line, token_len);
    buf[token_len] = '\0';

    return buf;
}

/** string const may include multiple words */
void extract_token_from_string_const(FILE *stream, Token *token)
{
    char line[LINE_MAX_LENGTH] = {0};
    char *buf = malloc(sizeof(*buf) * TOKEN_MAX_LENGTH);
    char *quote_start = NULL;
    char *quote_end = NULL;
    int str_len = 0;

    fgets(line, sizeof(line), stream);

    quote_start = line;
    quote_end = strchr(quote_start+1, '"');
    str_len = quote_end - (quote_start + 1);
    memcpy(buf, quote_start+1, str_len);
    buf[str_len] = '\0';

    token->type = STRING_CONST;
    strcpy(token->value, buf);

    free(buf);
}

void extract_token_from_word(Token *token, char *word)
{
    if (is_keyword(word)) {
        token->type = KEYWORD;
    } else if (is_int_const(word)) {
        token->type = INT_CONST;
    } else {
        token->type = IDENTIFIER;
    }

    strcpy(token->value, word);
}

int has_more_token(FILE *stream)
{
    int c = 0;

    while (!feof(stream)) {
        c = fgetc(stream);
        char c_next = fgetc(stream); // Peek next char for making sure that is a comment
        ungetc(c_next, stream);

        int is_whitespaces = c == ' ' || c == '\t';
        int is_empty_line = c == '\n';
        int is_comment_begin = (c == '/') && (c_next == '/' || c_next == '*');

        if (is_whitespaces || is_empty_line) continue;

        if (is_comment_begin) {
            skip_to_line_after_comment_end(stream);
            continue;
        }

        ungetc(c, stream);
        break;
    }

    return feof(stream) ? 0 : 1;
}

void getToken(FILE *stream, Token *token)
{
    long og_pos = ftell(stream);

    if (is_string_const(stream)) {
        extract_token_from_string_const(stream, token);
        fseek(stream, og_pos + strlen(token->value) + 2, SEEK_SET); // +2 for skip " and "
        return;
    }

    char *word = get_word(stream);
    int symbol_i = get_first_symbol_index(word);

    switch (symbol_i) {
    case -1:
        fseek(stream, og_pos + strlen(word) + 1, SEEK_SET); // +1 for skip the ' ' or '\n'
        extract_token_from_word(token, word);
        break;
    case 0:
        token->type = SYMBOL;
        sprintf(token->value, "%c", word[0]);
        fseek(stream, og_pos+1, SEEK_SET);
        break;
    default:
        word[symbol_i] = '\0';
        fseek(stream, og_pos+symbol_i, SEEK_SET);
        extract_token_from_word(token, word);
        break;
    }

    free(word);
}
