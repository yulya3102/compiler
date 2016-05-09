#include "l.h"

#include <utils/undefined.h>
#include <utils/fmap.h>

namespace optimise
{
namespace
{
std::list<std::string> argument_names(const codegen::Function & func)
{
    std::list<std::string> result;
    for (auto arg : func.arguments)
        result.push_back(arg.name);

    return result;
}

struct Recursive
{
    Recursive(const codegen::Function & f)
        : f(f)
    {}

    const std::string & name() const
    {
        return f.name;
    }

    const codegen::Function & f;
};

struct TCO : Recursive
{
    TCO(const codegen::Function & func)
        : Recursive(func)
        , entry_label(func.name + "_entry")
        , arguments(argument_names(func))
    {}

    void optimise_statement(const ast::Assignment & st, std::list<codegen::Statement> & statements) const
    {
        statements.push_back(codegen::Statement(st));
    }

    boost::optional<ast::Call> get_recursive_call(const ast::Expression & expr) const
    {
        const ast::Call * call_ptr = boost::get<ast::Call>(&expr.expression);
        if (!call_ptr)
            return boost::none;

        const ast::Call & call = *call_ptr;
        const ast::Value * value_ptr = boost::get<ast::Value>(&call.function->expression);
        if (!value_ptr)
            return boost::none;

        const ast::Value & value = *value_ptr;
        const std::string & function_name = boost::get<std::string>(value.value);
        if (function_name != this->name())
            return boost::none;

        return call;
    }

    void optimise_statement(const codegen::If & st, std::list<codegen::Statement> & statements) const
    {
        codegen::If result(st);
        result.thenBody.clear();
        result.elseBody.clear();

        for (auto statement : st.thenBody)
            fmap([&], x, this->optimise_statement(x, result.thenBody),
                 statement.statement);
        for (auto statement : st.elseBody)
            fmap([&], x, this->optimise_statement(x, result.elseBody),
                 statement.statement);

        statements.push_back(codegen::Statement(result));
    }

    void optimise_statement(const codegen::While & st, std::list<codegen::Statement> & statements) const
    {
        codegen::While result(st);
        result.body.clear();

        for (auto statement : st.body)
            fmap([&], x, this->optimise_statement(x, result.body),
                 statement.statement);

        statements.push_back(codegen::Statement(result));
    }

    void optimise_statement(const codegen::Continue & st, std::list<codegen::Statement> & statements) const
    {
        statements.push_back(codegen::Statement(st));
    }

    void optimise_statement(const ast::Write & st, std::list<codegen::Statement> & statements) const
    {
        statements.push_back(codegen::Statement(st));
    }

    void optimise_statement(const ast::Return & st, std::list<codegen::Statement> & statements) const
    {
        boost::optional<ast::Call> tail_call = get_recursive_call(st.expr);
        if (!tail_call)
        {
            statements.push_back(codegen::Statement(st));
            return;
        }

        auto formal_arg_it = this->arguments.begin();
        for (const ast::Expression & arg : tail_call->arguments)
        {
            ast::Value arg_var(*formal_arg_it);
            ast::Expression lval(arg_var);
            ast::Assignment statement{arg.loc, lval, arg};
            statements.push_back(codegen::Statement(statement));
            ++formal_arg_it;
        }
        assert(formal_arg_it == this->arguments.end());

        codegen::Continue statement{tail_call->loc, this->entry_label};
        statements.push_back(codegen::Statement(statement));
    }

    codegen::Function optimise() const
    {
        codegen::Function result(f);
        std::list<codegen::Statement> statements;

        for (auto st : f.statements)
            fmap([&], x, this->optimise_statement(x, statements), st.statement);

        codegen::While body{f.loc, entry_label,
                            ast::Const(true),
                            statements};
        result.statements.clear();
        result.statements.push_back(codegen::Statement(body));

        return result;
    }

    std::string entry_label;
    std::list<std::string> arguments;
};

void optimise_tail_call(codegen::Function & func)
{
    TCO tco(func);
    func = tco.optimise();
}
}

void optimise_tail_call(codegen::Code & code)
{
    optimise_functions([] (codegen::Function & f)
    {
        optimise_tail_call(f);
        return std::list<codegen::Function>();
    }, code);
}
}
