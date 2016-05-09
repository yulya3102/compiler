#include "l.h"

#include <utils/undefined.h>
#include <utils/fmap.h>

namespace optimise
{
namespace
{
void get_returns(const codegen::Statement & st, std::list<ast::Expression> & returns)
{
    const ast::Return * ret = boost::get<ast::Return>(&st.statement);
    if (ret)
    {
        returns.push_back(ret->expr);
        return;
    }

    const codegen::If * if_st = boost::get<codegen::If>(&st.statement);
    if (if_st)
    {
        for (auto s : if_st->thenBody)
            get_returns(s, returns);
        for (auto s : if_st->elseBody)
            get_returns(s, returns);
        return;
    }

    const codegen::While * while_st = boost::get<codegen::While>(&st.statement);
    if (while_st)
    {
        for (auto s : while_st->body)
            get_returns(s, returns);
        return;
    }
}

std::list<ast::Expression> get_returns(const codegen::Function & f)
{
    std::list<ast::Expression> result;

    for (auto st : f.statements)
        get_returns(st, result);

    return result;
}

bool calls_function(const ast::Expression & expr, const std::string & function_name);

bool calls_function(const ast::Value & expr, const std::string & function_name)
{
    return false;
}

bool calls_function(const ast::BinOperator & expr, const std::string & function_name)
{
    return calls_function(*expr.lhs, function_name)
        || calls_function(*expr.rhs, function_name);
}

bool calls_function(const ast::Dereference & expr, const std::string & function_name)
{
    return calls_function(*expr.expr, function_name);
}

bool calls_function(const ast::Address & expr, const std::string & function_name)
{
    return calls_function(*expr.expr, function_name);
}

bool calls_function(const ast::Call & expr, const std::string & function_name)
{
    const ast::Value * f = boost::get<ast::Value>(&expr.function->expression);
    if (f)
    {
        const std::string * name = boost::get<std::string>(&f->value);
        if (name && (*name == function_name))
            return true;
    }

    for (auto arg : expr.arguments)
        if (calls_function(arg, function_name))
            return true;

    return false;
}

bool calls_function(const ast::Read & expr, const std::string & function_name)
{
    return false;
}

bool calls_function(const ast::Expression & expr, const std::string & function_name)
{
    return fmap([&function_name], x, calls_function(x, function_name),
                expr.expression);
}

std::list<ast::Expression> get_non_recursive_returns(const codegen::Function & f)
{
    std::list<ast::Expression> returns = get_returns(f);

    returns.remove_if([f] (const ast::Expression & e)
                      { return calls_function(e, f.name); });

    return returns;
}

std::list<ast::Expression> get_recursive_returns(const codegen::Function & f)
{
    std::list<ast::Expression> returns = get_returns(f);

    returns.remove_if([f] (const ast::Expression & e)
                      { return !calls_function(e, f.name); });

    return returns;
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
