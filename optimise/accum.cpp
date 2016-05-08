#include "l.h"

#include <utils/undefined.h>

namespace optimise
{
namespace
{
std::list<ast::Expression> get_non_recursive_returns(const codegen::Function & f)
{
    undefined;
}

std::list<ast::Expression> get_recursive_returns(const codegen::Function & f)
{
    undefined;
}

void rewrite_returns_to_acc(codegen::Function & f)
{
    undefined;
}

std::list<codegen::Function> optimise(codegen::Function & f)
{
    std::list<ast::Expression> non_recursive = get_non_recursive_returns(f);
    std::list<ast::Expression> recursive = get_recursive_returns(f);

    if (non_recursive.size() != 1)
        return {};
    if (recursive.size() != 1)
        return {};

    ast::Expression init_acc = non_recursive.front();
    codegen::Function func_acc(f);
    func_acc.arguments.push_back({f.loc, f.type, "_accumulator"});
    f.statements.clear();
    f.statements.push_back(
        codegen::Statement(ast::Return{f.loc, init_acc})
    );
    rewrite_returns_to_acc(func_acc);
    return { func_acc };
}
}

void optimise_to_accum(codegen::Code & code)
{
    optimise_functions(optimise, code);
}
}
