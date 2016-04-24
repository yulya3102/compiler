#include "lcc.h"

#include <parse/parser.h>
#include <parse/ast/l.h>
#include <gen/gen.h>
#include <sem/l.h>
#include <optimise/l.h>

#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <cstdlib>

void lcc::compile_llvm(std::istream & in, llvm::raw_ostream & out, const std::string & module_name)
{
    ast::parser p;
    ast::Code code = p.parse(in, std::cout);
    sem::verify(code);
    optimise::optimise_tail_call(code);
    codegen::prepare(code);
    std::unique_ptr<llvm::Module> module = codegen::generate(code, module_name.c_str());

    module->print(out, nullptr);
}

std::string lcc::create_temp_file(const char * pattern, std::size_t suffix_size)
{
    char filename[strlen(pattern) + 1];
    strcpy(filename, pattern);
    int tmp_fd = mkstemps(filename, suffix_size);
    close(tmp_fd);
    return filename;
}

void lcc::compile_executable(std::istream & in, const std::string & output_name)
{
    std::string ll_filename = create_temp_file("lcc_XXXXXX.ll", 3);
    {
        std::error_code err;
        llvm::raw_fd_ostream out(ll_filename, err, llvm::sys::fs::OpenFlags::F_None);
        compile_llvm(in, out, output_name);
    }

    std::string object_filename = create_temp_file("lcc_XXXXXX.o", 2);
    {
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
