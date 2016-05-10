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

boost::optional<ast::Expression &> get_call_to(ast::Expression & expr, const std::string & f)
{
    if (boost::get<ast::Value>(&expr.expression)
        || boost::get<ast::Read>(&expr.expression))
        return boost::none;

    auto * op = boost::get<ast::BinOperator>(&expr.expression);
    if (op)
    {
        auto lhs_call = get_call_to(*op->lhs, f);
        if (lhs_call)
            return lhs_call;
        return get_call_to(*op->rhs, f);
    }

    auto * deref = boost::get<ast::Dereference>(&expr.expression);
    if (deref)
        return get_call_to(*deref->expr, f);

    auto * addr = boost::get<ast::Address>(&expr.expression);
    if (addr)
        return get_call_to(*addr->expr, f);

    ast::Call & call = boost::get<ast::Call>(expr.expression);
    if (is_name(*call.function, f))
        return expr;

    for (auto arg : call.arguments)
    {
        auto arg_call = get_call_to(arg, f);
        if (arg_call)
            return arg_call;
    }

    return boost::none;
}

bool calls_function(const ast::Expression & expr, const std::string & function_name)
{
    return static_cast<bool>(get_call_to(const_cast<ast::Expression&>(expr), function_name));
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

    std::string accumulated_function_name() const
    {
        return name() + "_accumulated";
    }

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
            boost::optional<ast::Expression &> maybe_rec_call = get_call_to(return_value, name());
            if (!maybe_rec_call)
            {
                return_value.expression = ast::Value(accumulator_variable(f).name);
                return;
            }

            ast::Expression & rec_call = *maybe_rec_call;
            ast::Call new_return_value(boost::get<ast::Call>(rec_call.expression));
            rec_call = ast::Value(accumulator_variable(f).name);
            assert(is_name(*new_return_value.function, f.name));
            new_return_value.function.reset(new ast::Expression(ast::Value(accumulated_function_name())));
            new_return_value.arguments.push_back(return_value);
            return_value = new_return_value;
            return;
        }

        codegen::If * if_st = boost::get<codegen::If>(&st.statement);
        if (if_st)
        {
            for (codegen::Statement & s : if_st->thenBody)
                rewrite_returns_to_acc(s);
            for (codegen::Statement & s : if_st->elseBody)
                rewrite_returns_to_acc(s);
            return;
        }

        codegen::While * while_st = boost::get<codegen::While>(&st.statement);
        if (while_st)
        {
            for (codegen::Statement & s : while_st->body)
                rewrite_returns_to_acc(s);
            return;
        }
    }

    void rewrite_returns_to_acc()
    {
        for (codegen::Statement & st : f.statements)
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
        std::list<ast::Expression> init_args;
        for (auto arg : f.arguments)
            init_args.push_back(ast::Expression(ast::Value(arg.name)));
        init_args.push_back(init_acc);
        ast::Expression init_call(ast::Call{
            f.loc,
            std::shared_ptr<ast::Expression>(
                new ast::Expression(
                    ast::Value(accumulated_function_name()))),
            init_args
        });
        f.statements.push_back(
            codegen::Statement(ast::Return{
                                   f.loc,
                                   init_call})
        );
        Accum accum(func_acc);
        accum.rewrite_returns_to_acc();
        func_acc.name = accumulated_function_name();
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
