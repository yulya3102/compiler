#pragma once

#include <ast/l.h>

#include <llvm/IR/Module.h>

#include <memory>

namespace codegen
{
std::unique_ptr<llvm::Module> generate(const ast::Code & code);
}
