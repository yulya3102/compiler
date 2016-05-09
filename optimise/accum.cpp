#include "l.h"
#include "recursive.h"

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

bool is_name(const ast::Expression & expr, const std::string & name)
{
    const ast::Value * v = boost::get<ast::Value>(&expr.expression);
    if (!v)
        return false;

    const std::string * n = boost::get<std::string>(&v->value);
    if (!n)
        return false;

    return *n == name;
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
    if (is_name(expr.function->expression, function_name))
        return true;

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

ast::Expression & get_call_to(ast::Expression & expr, const std::string & f)
{
    undefined;
}

codegen::Variable accumulator_variable(const codegen::Function & f)
{
    return {f.loc, f.type, "_accumulator"};
}

struct Accum : Recursive
{
    Accum(codegen::Function & f)
        : Recursive(f)
    {}

    std::list<ast::Expression> get_non_recursive_returns() const
    {
        std::list<ast::Expression> returns = get_returns(f);

        returns.remove_if([this] (const ast::Expression & e)
                          { return calls_function(e, f.name); });

        return returns;
    }

    std::list<ast::Expression> get_recursive_returns() const
    {
        std::list<ast::Expression> returns = get_returns(f);

        returns.remove_if([this] (const ast::Expression & e)
                          { return !calls_function(e, f.name); });

        return returns;
    }

    void rewrite_returns_to_acc(codegen::Statement & st)
    {
        ast::Return * ret = boost::get<ast::Return>(&st.statement);
        if (ret)
        {
            ast::Expression & return_value = ret->expr;
            std::string function_name = undefined_expr(std::string);
            ast::Expression & rec_call = get_call_to(return_value, function_name);
            ast::Call new_return_value(boost::get<ast::Call>(rec_call.expression));
            rec_call = ast::Value(accumulator_variable(undefined_expr(codegen::Function)).name);
            new_return_value.arguments.push_back(return_value);
            return_value = new_return_value;
            return;
        }

        codegen::If * if_st = boost::get<codegen::If>(&st.statement);
        if (if_st)
        {
            for (auto s : if_st->thenBody)
                rewrite_returns_to_acc(s);
            for (auto s : if_st->elseBody)
                rewrite_returns_to_acc(s);
            return;
        }

        codegen::While * while_st = boost::get<codegen::While>(&st.statement);
        if (while_st)
        {
            for (auto s : while_st->body)
                rewrite_returns_to_acc(s);
            return;
        }
    }

    void rewrite_returns_to_acc()
    {
        for (auto st : f.statements)
            rewrite_returns_to_acc(st);
    }

    std::list<codegen::Function> optimise()
    {
        std::list<ast::Expression> non_recursive = get_non_recursive_returns();
        std::list<ast::Expression> recursive = get_recursive_returns();

        if (non_recursive.size() != 1)
            return {};
        if (recursive.size() != 1)
            return {};
        if (is_name(recursive.front(), f.name))
            return {};

        ast::Expression init_acc = non_recursive.front();
        codegen::Function func_acc(f);
        func_acc.arguments.push_back(accumulator_variable(f));
        f.statements.clear();
        f.statements.push_back(
            codegen::Statement(ast::Return{f.loc, init_acc})
        );
        Accum accum(func_acc);
        accum.rewrite_returns_to_acc();
        return { func_acc };
    }
};

std::list<codegen::Function> optimise(codegen::Function & f)
{
    Accum accum(f);
    return accum.optimise();
}
}

void optimise_to_accum(codegen::Code & code)
{
    optimise_functions(optimise, code);
}
}
