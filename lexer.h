#pragma once

#undef yyFlexLexer
#include <FlexLexer.h>

struct lexer_t;

#include "ast_parser.h"
#include <ast/l.h>
#include "parser.h"

struct lexer_t : yyFlexLexer
{
    typedef ast::parser::token_type token;

    token lex(ast::parser::semantic_type * yylval,
              ast::parser::location_type * yylloc);
};
