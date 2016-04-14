#include <parse/parser.h>
#include <parse/ast/l.h>
#include <gen/gen.h>
#include <sem/l.h>

#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <fstream>

int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input> <output>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream in(argv[1]);

    ast::parser p;
    ast::Code code = p.parse(in, std::cout);
    sem::verify(code);
    std::unique_ptr<llvm::Module> module = codegen::generate(code, argv[1]);

    std::error_code err;
    llvm::raw_fd_ostream out(argv[2], err, llvm::sys::fs::OpenFlags::F_None);
    module->print(out, nullptr);
}
