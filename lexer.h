#pragma once

#include <FlexLexer.h>

struct lexer_t;

#include "parser.h"

struct lexer_t : yyFlexLexer
{
    yy::parser::token_type lex(
            yy::parser::semantic_type * yylval,
            yy::parser::location_type * yylloc);
};
