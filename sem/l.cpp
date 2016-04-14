#include "l.h"
#include "types.h"

#include <utils/undefined.h>
#include <utils/fmap.h>
#include <utils/top.h>

#include <boost/variant.hpp>

#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <sstream>

namespace sem
{
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

void verify(const typed_ctx<top> & ctx, const ast::Declaration & entry)
{
    undefined;
}

void verify(const typed_ctx<top> & ctx, const ast::VarDeclaration & entry)
{
    undefined;
}

void verify(const typed_ctx<top> & ctx, const ast::Statement & entry);

void verify(const typed_ctx<top> & ctx, const ast::Skip & entry)
{
    undefined;
}

void verify(const typed_ctx<top> & ctx, const ast::Assignment & entry)
{
    undefined;
}

void verify(const typed_ctx<top> & ctx, const ast::Seq & entry)
{
    verify(ctx, *entry.first);
    verify(ctx, *entry.second);
}

void verify(const typed_ctx<top> & ctx, const ast::If & entry)
{
    undefined;
}

void verify(const typed_ctx<top> & ctx, const ast::While & entry)
{
    undefined;
}

void verify(const typed_ctx<top> & ctx, const ast::Read & entry)
{
    undefined;
}

void verify(const typed_ctx<top> & ctx, const ast::Write & entry)
{
    undefined;
}

void verify(const typed_ctx<top> & ctx, const ast::Return & entry)
{
    undefined;
}

void verify(const typed_ctx<top> & ctx, const ast::Statement & entry)
{
    return fmap([&ctx], x, verify(ctx, x), entry.statement);
}

void verify(const typed_ctx<top> & ctx, const ast::FuncDefinition & entry)
{
    typed_ctx<top> inner_scope(ctx);
    for (auto & arg : entry.declaration.arguments)
        inner_scope.declare({arg.type, top()}, arg.name);
    return verify(inner_scope, entry.statement);
}

void verify(const typed_ctx<top> & ctx, const ast::Definition & entry)
{
    return fmap([&ctx], x, verify(ctx, x), entry.definition);
}

void verify(const typed_ctx<top> & ctx, const ast::CodeEntry & entry)
{
    return fmap([&ctx], x, verify(ctx, x), entry.entry);
}

void verify(const ast::Code & code)
{
    typed_ctx<top> ctx;
    context<top> definitions;
    for (auto entry : code.entries)
    {
        auto entry_name = name(entry);
        ctx.declare({ ctx.get_type(entry), top() }, entry_name);
        if (is_definition(entry))
        {
            if (definitions.is_declared(entry_name))
                throw std::runtime_error("entry '" + entry_name + "' was already defined");
            else
                definitions.declare(top(), entry_name);
        }
        verify(ctx, entry);
    }


}

}
