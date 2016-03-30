#include <parse/parser.h>
#include <ast/l.h>
#include "codegen.h"

#include <iostream>
#include <fstream>

int main(int argc, char ** argv)
{
    std::ifstream in;
    if (argc == 2)
        in.open(argv[1]);

    ast::parser p;
    ast::Code code = p.parse(
                in.is_open() ? in : std::cin,
                std::cout);
    codegen::generate(code)->dump();
}
