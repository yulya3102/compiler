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

    ast::Assignment optimise_statement(const ast::Assignment & st) const
    {
        return st;
    }

    bool is_tail_call(const ast::Value & expr) const
    {
        return false;
    }

    bool is_tail_call(const ast::BinOperator & expr) const
    {
        return false;
    }

    bool is_tail_call(const ast::Dereference & expr) const
    {
        return false;
    }

    bool is_tail_call(const ast::Address & expr) const
    {
        return false;
    }

    bool is_tail_call(const ast::Call & expr) const
    {
        undefined;
    }

    bool is_tail_call(const ast::Read & expr) const
    {
        return false;
    }

    bool is_tail_call(const ast::Expression & expr) const
    {
        return fmap([this], x, this->is_tail_call(x), expr.expression);
    }

    codegen::If optimise_statement(const codegen::If & st) const
    {
        codegen::If result(st);
        result.thenBody.clear();
        result.elseBody.clear();

        for (auto statement : st.thenBody)
            result.thenBody.push_back(fmap([this], x,
                                      codegen::Statement(this->optimise_statement(x)),
                                      statement.statement));
        for (auto statement : st.elseBody)
            result.elseBody.push_back(fmap([this], x,
                                      codegen::Statement(this->optimise_statement(x)),
                                      statement.statement));

        return result;
    }

    codegen::While optimise_statement(const codegen::While & st) const
    {
        codegen::While result(st);
        result.body.clear();

        for (auto statement : st.body)
            result.body.push_back(fmap([this], x,
                                  codegen::Statement(this->optimise_statement(x)),
                                  statement.statement));

        return result;
    }

    codegen::Continue optimise_statement(const codegen::Continue & st) const
    {
        return st;
    }

    ast::Write optimise_statement(const ast::Write & st) const
    {
        return st;
    }

    ast::Return optimise_statement(const ast::Return & st) const
    {
        if (is_tail_call(*st.expr))
            undefined;
        return st;
    }

    codegen::Function optimise() const
    {
        codegen::Function result(func);
        std::list<codegen::Statement> statements;

        for (auto st : func.statements)
            statements.push_back(fmap([this], x,
                                      codegen::Statement(this->optimise_statement(x)),
                                      st.statement));

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
