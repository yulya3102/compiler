#include "lcc.h"

#include <parse/parser.h>
#include <parse/ast/l.h>
#include <gen/gen.h>
#include <sem/l.h>

#include <llvm/IR/Module.h>

void lcc::compile(std::istream & in, llvm::raw_ostream & out, const std::string & module_name)
{
    ast::parser p;
    ast::Code code = p.parse(in, std::cout);
    sem::verify(code);
    std::unique_ptr<llvm::Module> module = codegen::generate(code, module_name.c_str());

    module->print(out, nullptr);
}
