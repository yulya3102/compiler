#pragma once

#include "c.h"

struct lexer_t;

namespace ast
{

struct ast_parser
{
    Code parse();

    Code code;
    lexer_t * lexer;
};

}
