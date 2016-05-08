#include <gen/ast/l.h>

#include <functional>

namespace optimise
{
void optimise_functions(
    std::function<std::list<codegen::Function>(codegen::Function &)> optimisation,
    codegen::Code & code);

void optimise_to_accum(codegen::Code & code);
void optimise_tail_call(codegen::Code & code);
}
