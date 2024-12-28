#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "type.h"
#include <stdio.h>

int has_more_token(FILE *stream);
void getToken(FILE *stream, Token *token);

#endif
