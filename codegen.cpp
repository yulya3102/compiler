#include "codegen.h"

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

#include <memory>

namespace codegen
{
std::unique_ptr<llvm::Module> generate(const ast::Code & code)
{
    std::unique_ptr<llvm::Module> result(new llvm::Module("abc", llvm::getGlobalContext()));
    return std::move(result);
}
}
