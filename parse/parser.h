#pragma once

#include <ast/l.h>

#include <iostream>

struct lexer_t;

namespace ast
{

struct parser
{
    Code parse(std::istream & in, std::ostream & out);

    Code code;
    lexer_t * lexer;
};

}
