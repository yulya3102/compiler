#include "l.h"

#include <utils/undefined.h>

#include <boost/variant.hpp>

#include <unordered_map>
#include <unordered_set>
#include <iostream>

#define fmap(closure, x, expr, variant) boost::apply_visitor(closure (const auto & x) { return expr; }, variant)

namespace
{
struct context
{
    void declare(const std::string & name, const ast::Type & type)
    {
        undefined;
    }

    void define(const std::string & name, const ast::Type & type)
    {
        undefined;
    }

    std::unordered_map<std::string, ast::Type> declared;
    std::unordered_set<std::string> defined;
    context * parent;
};
}

namespace sem
{

ast::Type type(const ast::Expression & expr)
{
    undefined;
}

ast::Type type(const ast::Declaration & entry)
{
    undefined;
}

ast::Type type(const ast::VarDeclaration & entry)
{
    return entry.type;
}

ast::Type type(const ast::FuncDeclaration & entry)
{
    std::list<ast::Type> args;
    for (auto & arg : entry.arguments)
        args.push_back(type(arg));
    return ast::FuncType{std::make_shared<ast::Type>(entry.type), args};
}

ast::Type type(const ast::FuncDefinition & entry)
{
    return type(entry.declaration);
}

ast::Type type(const ast::Definition & entry)
{
    return fmap([], x, type(x), entry.definition);
}

ast::Type type(const ast::CodeEntry & entry)
{
    return fmap([], x, type(x), entry.entry);
}

std::string name(const ast::Declaration & entry)
{
    undefined;
}

std::string name(const ast::VarDeclaration & entry)
{
    return entry.name;
}

std::string name(const ast::FuncDeclaration & entry)
{
    return entry.name;
}

std::string name(const ast::FuncDefinition & entry)
{
    return name(entry.declaration);
}

std::string name(const ast::Definition & entry)
{
    return fmap([], x, name(x), entry.definition);
}

std::string name(const ast::CodeEntry & entry)
{
    return fmap([], x, name(x), entry.entry);
}

ast::location location(const ast::CodeEntry & entry)
{
    undefined;
}

bool is_definition(const ast::Declaration & entry)
{
    return false;
}

bool is_definition(const ast::Definition & entry)
{
    return true;
}

bool is_definition(const ast::CodeEntry & entry)
{
    return fmap([], x, is_definition(x), entry.entry);
}

bool verify(const context & ctx, const ast::Declaration & entry)
{
    return true;
}

bool verify(const context & ctx, const ast::VarDeclaration & entry)
{
    return true;
}

bool verify(const context & ctx, const ast::Statement & entry)
{
    undefined;
}

bool verify(const context & ctx, const ast::FuncDefinition & entry)
{
    context inner_scope(ctx);
    for (auto & arg : entry.declaration.arguments)
        inner_scope.define(arg.name, arg.type);
    return verify(inner_scope, entry.statement);
}

bool verify(const context & ctx, const ast::Definition & entry)
{
    return fmap([&ctx], x, verify(ctx, x), entry.definition);
}

bool verify(const context & ctx, const ast::CodeEntry & entry)
{
    return fmap([&ctx], x, verify(ctx, x), entry.entry);
}

bool verify(const ast::Code & code)
{
    context ctx;
    for (auto entry : code.entries)
    {
        std::string n = name(entry);
        if (ctx.declared.find(n) != ctx.declared.end())
        {
            if (ctx.declared.at(n) != type(entry))
            {
                std::cerr << "error at " << location(entry)
                          << ": symbol '" << n
                          << "' redeclared with different type"
                          << std::endl;
                return false;
            }

            if (ctx.defined.find(n) != ctx.defined.end() && is_definition(entry))
            {
                std::cerr << "error at " << location(entry)
                          << ": symbol '" << n
                          << "' was already defined"
                          << std::endl;
                return false;
            }
        }
        else
            ctx.declared.emplace(name(entry), type(entry));

        if (!verify(ctx, entry))
            return false;

        if (is_definition(entry))
            ctx.defined.insert(name(entry));
    }
    return true;
}

}
