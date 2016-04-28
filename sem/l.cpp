#include "l.h"
#include "types.h"
#include "func.h"
#include "error.h"

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
std::string name(const ast::FuncDeclaration & entry);

std::string name(const ast::Declaration & entry)
{
    return fmap([], x, name(x), entry.declaration);
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
    // Correctness of redeclaration was already checked in verify(code)
    // and entry was already declared

    // Do nothing
}

void verify(const typed_ctx<top> & ctx, const ast::VarDeclaration & entry)
{
    // Correctness of redeclaration was already checked in verify(code)
    // and entry was already declared

    // Do nothing
}

void verify(const typed_ctx<top> & ctx, const ast::FuncDefinition & entry)
{
    function_ctx inner_scope(entry.declaration.type, &ctx);
    for (auto & arg : entry.declaration.arguments)
        inner_scope.declare({arg.type, top()}, arg.name);
    inner_scope.verify_statement(entry.statements);
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
                throw semantic_error(entry.loc, "entry '" + entry_name + "' was already defined");
            else
                definitions.declare(top(), entry_name);
        }
        verify(ctx, entry);
    }


}

}
