#pragma once

#include "ctx.h"
#include "error.h"

#include <parse/ast/l.h>

#include <utils/undefined.h>
#include <utils/fmap.h>

#include <boost/variant/get.hpp>

namespace sem
{
void expect_type(const ast::Type & real, const ast::Type & expected, const std::string & error_message);
ast::Type binop_expected_argtype(ast::Oper::OperName op);
ast::Type binop_rettype(ast::Oper::OperName op);

template <typename T>
struct typed_ctx : context<std::pair<ast::Type, T>>
{
    typed_ctx(const typed_ctx * outer_scope = nullptr)
        : context<std::pair<ast::Type, T>>(outer_scope)
    {}

    ast::Type get_type(int64_t) const
    {
        return ast::int_type();
    }

    ast::Type get_type(bool) const
    {
        return ast::bool_type();
    }

    ast::Type get_type(const ast::Const & expr) const
    {
        return fmap([this], x, this->get_type(x), expr.constant);
    }

    ast::Type get_type(const std::string & expr) const
    {
        return this->get(expr).first;
    }

    ast::Type get_type(const ast::Value & expr) const
    {
        return fmap([this], x, this->get_type(x), expr.value);
    }

    ast::Type get_type(const ast::BinOperator & expr) const
    {
        return binop_rettype(expr.oper.oper);
    }

    ast::Type get_type(const ast::Dereference & expr) const
    {
        auto type = this->get_type(*expr.expr);

        auto dereferenced_type = boost::get<ast::PointerType>(&type.type);
        if (!dereferenced_type)
            throw semantic_error(expr.loc, "dereference of non-pointer type is illegal");

        return *(dereferenced_type->type);
    }

    ast::Type get_type(const ast::Address & expr) const
    {
        return ast::pointer_type(this->get_type(*expr.expr), expr.loc);
    }

    ast::Type get_type(const ast::Call & expr) const
    {
        ast::Type func_type = this->get_type(*expr.function);
        return *boost::get<ast::FuncType>(func_type.type).rettype;
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

    ast::Type get_type(const ast::Declaration & entry) const
    {
        return fmap([this], x, this->get_type(x), entry.declaration);
    }

    ast::Type get_type(const ast::VarDeclaration & entry) const
    {
        return entry.type;
    }

    ast::Type get_type(const ast::FuncDefinition & entry) const
    {
        return this->get_type(entry.declaration);
    }

    ast::Type get_type(const ast::Definition & entry) const
    {
        return fmap([this], x, this->get_type(x), entry.definition);
    }

    ast::Type get_type(const ast::CodeEntry & entry) const
    {
        return fmap([this], x, this->get_type(x), entry.entry);
    }
};
}
