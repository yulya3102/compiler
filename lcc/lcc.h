#pragma once

#include <llvm/Support/raw_ostream.h>
#include <istream>

namespace lcc
{
void compile_llvm(std::istream & in, llvm::raw_ostream & out, const std::string & module_name);
void compile_executable(std::istream & in, const std::string & output_name);
std::string create_temp_file(const char * pattern, std::size_t suffix_size = 0);
std::string get_env_variable(const char * varname, const char * default_value);
}
