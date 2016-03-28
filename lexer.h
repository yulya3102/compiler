#pragma once

#undef yyFlexLexer
#include <FlexLexer.h>

struct lexer_t;

#include "parser.h"

struct lexer_t : yyFlexLexer
{
    typedef yy::parser::token_type token;

    token lex(yy::parser::semantic_type * yylval,
              yy::parser::location_type * yylloc);
};
