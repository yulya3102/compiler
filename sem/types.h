#pragma once

#include "ctx.h"

#include <parse/ast/l.h>

#include <utils/undefined.h>

namespace sem
{
template <typename T>
struct typed_ctx : context<std::pair<ast::Type, T>>
{
    typed_ctx(typed_ctx * outer_scope = nullptr)
        : context<std::pair<ast::Type, T>>(outer_scope)
    {}

    ast::Type get_type(const ast::Expression & expr) const
    {
        undefined;
    }

    ast::Type get_type(const ast::FuncDeclaration & expr) const
    {
        undefined;
    }
};
}
