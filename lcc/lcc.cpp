#include "lcc.h"

#include <parse/parser.h>
#include <parse/ast/l.h>
#include <gen/gen.h>
#include <sem/l.h>

#include <llvm/IR/Module.h>
#include <cstdlib>

void lcc::compile_llvm(std::istream & in, llvm::raw_ostream & out, const std::string & module_name)
{
    ast::parser p;
    ast::Code code = p.parse(in, std::cout);
    sem::verify(code);
    std::unique_ptr<llvm::Module> module = codegen::generate(code, module_name.c_str());

    module->print(out, nullptr);
}

void lcc::compile_executable(std::istream & in, const std::string & output_name)
{
    char ll_filename[255];
    {
        strcpy(ll_filename, "lcc_XXXXXX.ll");
        int tmp_fd = mkstemps(ll_filename, 3);
        llvm::raw_fd_ostream out(tmp_fd, true);
        compile_llvm(in, out, output_name);
    }

    char object_filename[255];
    {
        strcpy(object_filename, "lcc_XXXXXX.o");
        int tmp_fd = mkstemps(object_filename, 2);
        close(tmp_fd);
        std::string command("llc -o=");
        command += object_filename;
        command += " --filetype=obj ";
        command += ll_filename;
        std::system(command.c_str());
    }

    {
        std::string command("gcc -o ");
        command += output_name;
        command += " ";
        command += object_filename;
        std::system(command.c_str());
    }
}
