#include <parse/parser.h>
#include <ast/l.h>
#include "codegen.h"

#include <iostream>

int main()
{
    ast::parser p;
    ast::Code code = p.parse(std::cin, std::cout);
    codegen::generate(code)->dump();
}
