#pragma once

#include <llvm/Support/raw_ostream.h>
#include <istream>

namespace lcc
{
void compile_llvm(std::istream & in, llvm::raw_ostream & out, const std::string & module_name);
void compile_executable(std::istream & in, const std::string & output_name);
}
