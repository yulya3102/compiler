#pragma once

#undef yyFlexLexer
#include <FlexLexer.h>

struct lexer_t;

#include "parser.h"
#include <ast/l.h>
#include "l.h"

struct lexer_t : yyFlexLexer
{
    typedef ast::generated_parser::token_type token;

    token lex(ast::generated_parser::semantic_type * yylval,
              ast::generated_parser::location_type * yylloc);
};
