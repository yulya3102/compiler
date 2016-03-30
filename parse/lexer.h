#pragma once

#undef yyFlexLexer
#include <FlexLexer.h>

#include "l.h"

#include <iostream>

struct lexer_t : yyFlexLexer
{
    typedef ast::generated_parser::token_type token;

    lexer_t(std::istream & in, std::ostream & out)
        : yyFlexLexer(&in, &out)
    {}

    token lex(ast::generated_parser::semantic_type * yylval,
              ast::generated_parser::location_type * yylloc);
};
