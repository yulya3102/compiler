#pragma once

#include "ctx.h"

#include <parse/ast/l.h>

#include <utils/undefined.h>
#include <utils/fmap.h>

namespace sem
{
template <typename T>
struct typed_ctx : context<std::pair<ast::Type, T>>
{
    typed_ctx(typed_ctx * outer_scope = nullptr)
        : context<std::pair<ast::Type, T>>(outer_scope)
    {}

    ast::Type get_type(const ast::Value & expr) const
    {
        undefined;
    }

    ast::Type get_type(const ast::BinOperator & expr) const
    {
        undefined;
    }

    ast::Type get_type(const ast::Dereference & expr) const
    {
        undefined;
    }

    ast::Type get_type(const ast::Call & expr) const
    {
        undefined;
    }

    ast::Type get_type(const ast::Expression & expr) const
    {
        return fmap([this], x, this->get_type(x), expr.expression);
    }

    ast::Type get_type(const ast::FuncDeclaration & expr) const
    {
        std::list<ast::Type> args;
        for (auto arg : expr.arguments)
            args.push_back(arg.type);
        std::shared_ptr<ast::Type> rettype(new ast::Type(expr.type));
        return ast::FuncType{expr.loc, rettype, args};
    }
};
}
