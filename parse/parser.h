#pragma once

#include <ast/l.h>

struct lexer_t;

namespace ast
{

struct parser
{
    Code parse();

    Code code;
    lexer_t * lexer;
};

}
