#pragma once

#include <parse/ast/l.h>

#include <boost/variant.hpp>

#include <list>
#include <string>
#include <memory>

namespace codegen
{
struct Statement
{
    template <typename T>
    explicit Statement(const T & t)
        : loc(t.loc)
        , statement(t)
    {}

    std::shared_ptr<ast::location> loc;
    boost::variant<
        ast::Assignment,
        ast::If,
        ast::While,
        ast::Write,
        ast::Return
        > statement;
};

using Variable = ast::VarDeclaration;

std::list<Variable> function_variables(const ast::FuncDefinition & func);
std::list<Statement> function_statements(const ast::FuncDefinition & func);

struct Function
{
    Function(const ast::FuncDefinition & func)
        : loc(func.loc)
        , type(func.declaration.type)
        , name(func.declaration.name)
        , arguments(func.declaration.arguments)
        , variables(function_variables(func))
        , statements(function_statements(func))
    {}

    std::shared_ptr<ast::location> loc;
    ast::Type type;
    std::string name;
    std::list<Variable> arguments;
    std::list<Variable> variables;
    std::list<Statement> statements;
};

struct CodeEntry
{
    template <typename T>
    CodeEntry(const T & t)
        : loc(t.loc)
        , entry(t)
    {}

    std::shared_ptr<ast::location> loc;
    boost::variant<Variable, Function> entry;
};

std::list<CodeEntry> construct_entries(const std::list<ast::CodeEntry> & entries);

struct Code
{
    Code(const ast::Code & code)
        : loc(code.loc)
        , entries(construct_entries(code.entries))
    {}

    std::shared_ptr<ast::location> loc;
    std::list<CodeEntry> entries;
};
}
