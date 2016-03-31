#include <parse/parser.h>
#include <parse/ast/l.h>
#include <gen/gen.h>

#include <iostream>
#include <fstream>

int main(int argc, char ** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename> [<filename>...]" << std::endl;
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i)
    {
        std::ifstream in(argv[i]);

        ast::parser p;
        ast::Code code = p.parse(in, std::cout);
        codegen::generate(code, argv[i])->dump();
    }
}
