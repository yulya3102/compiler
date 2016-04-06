#include <parse/parser.h>
#include <parse/ast/l.h>
#include <gen/gen.h>
#include <sem/l.h>

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

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
        // sem::verify(code);
        std::unique_ptr<llvm::Module> module = codegen::generate(code, argv[i]);
        module->print(llvm::outs(), nullptr);
    }
}
