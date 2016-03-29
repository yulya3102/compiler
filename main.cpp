#include <parse/parser.h>
#include <ast/l.h>
#include "codegen.h"

#include <iostream>

int main()
{
    ast::parser p;
    ast::Code code = p.parse();
    codegen::generate(code)->dump();
}
