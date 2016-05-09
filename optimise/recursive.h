#include <gen/ast/l.h>

namespace optimise
{
struct Recursive
{
    Recursive(codegen::Function & f)
        : f(f)
    {}

    const std::string & name() const
    {
        return f.name;
    }

    codegen::Function & f;
};
}
