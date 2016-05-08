#include "l.h"

#include <utils/undefined.h>
#include <utils/fmap.h>

namespace optimise
{
namespace
{
void optimise_tail_call(codegen::Variable &)
{ }

std::list<std::string> argument_names(const codegen::Function & func)
{
    std::list<std::string> result;
    for (auto arg : func.arguments)
        result.push_back(arg.name);

    return result;
}

struct TCO
{
    TCO(const codegen::Function & func)
        : function_name(func.name)
        , entry_label(func.name + "_entry")
        , arguments(argument_names(func))
        , func(func)
    {}

    void optimise_statement(const ast::Assignment & st, std::list<codegen::Statement> & statements) const
    {
        statements.push_back(codegen::Statement(st));
    }

    bool is_tail_call(const ast::Expression & expr) const
    {
        const ast::Call * call_ptr = boost::get<ast::Call>(&expr.expression);
        if (!call_ptr)
            return false;

        const ast::Call & call = *call_ptr;
        const ast::Value * value_ptr = boost::get<ast::Value>(&call.function->expression);
        if (!value_ptr)
            return false;

        const ast::Value & value = *value_ptr;
        const std::string & function_name = boost::get<std::string>(value.value);
        return function_name == this->function_name;
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
        if (is_tail_call(*st.expr))
            undefined;
        statements.push_back(codegen::Statement(st));
    }

    codegen::Function optimise() const
    {
        codegen::Function result(func);
        std::list<codegen::Statement> statements;

        for (auto st : func.statements)
            fmap([&], x, this->optimise_statement(x, statements), st.statement);

        codegen::While body{func.loc, entry_label,
                            ast::Const(true),
                            statements};
        result.statements.clear();
        result.statements.push_back(codegen::Statement(body));

        return result;
    }

    std::string function_name;
    std::string entry_label;
    std::list<std::string> arguments;
    const codegen::Function & func;
};

void optimise_tail_call(codegen::Function & func)
{
    TCO tco(func);
    func = tco.optimise();
}
}

void optimise_tail_call(codegen::Code & code)
{
    for (codegen::CodeEntry & entry : code.entries)
        fmap([], x, optimise_tail_call(x), entry.entry);
}

}
