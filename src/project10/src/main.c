#include "type.h"
#include "tokenizer.h"
#include "compile_engine.h"
#include <stdio.h>

int main(void)
{
    FILE *i_stream = fopen("../provided_test/ExpressionLessSquare/SquareGame.jack", "r");
    FILE *o_stream = fopen("../provided_test/ExpressionLessSquare/TSquareGame.xml", "w");
    Token token = {0};

    fprintf(o_stream, "<tokens>\n");

    while (has_more_token(i_stream)) {
        getToken(i_stream, &token);
        compile_term(o_stream, &token);
    }

    fprintf(o_stream, "</tokens>");
}
