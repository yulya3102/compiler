#include "l.h"

#include <utils/undefined.h>

namespace optimise
{
namespace
{
std::list<ast::Value> get_non_recursive_returns(const codegen::Function & f)
{
    undefined;
}

std::list<ast::Value> get_recursive_returns(const codegen::Function & f)
{
    undefined;
}

void rewrite_returns_to_acc(codegen::Function & f)
{
    undefined;
}

std::list<codegen::Function> optimise(codegen::Function & f)
{
    std::list<ast::Value> non_recursive = get_non_recursive_returns(f);
    std::list<ast::Value> recursive = get_recursive_returns(f);

    if (non_recursive.size() != 1)
        return {};
    if (recursive.size() != 1)
        return {};

    ast::Value init_acc = non_recursive.front();
    codegen::Function func_acc(f);
    func_acc.arguments.push_back({f.loc, f.type, "_accumulator"});
    f.statements.clear();
    f.statements.push_back(
        codegen::Statement(
            ast::Return{
                f.loc,
                undefined_expr(std::shared_ptr<ast::Expression>)
            }
        )
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
