#pragma once

#include "ctx.h"

#include <parse/ast/l.h>

#include <utils/undefined.h>
#include <utils/fmap.h>

namespace sem
{
void expect_type(const ast::Type & real, const ast::Type & expected, const std::string & error_message);

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
        auto expected_arg_type = [] (ast::Oper::OperName op)
        {
            switch (op)
            {
                case ast::Oper::PLUS:
                case ast::Oper::MINUS:
                case ast::Oper::MULT:
                case ast::Oper::DIV:
                case ast::Oper::MOD:
                case ast::Oper::GT:
                case ast::Oper::LT:
                case ast::Oper::EQ:
                case ast::Oper::GE:
                case ast::Oper::LE:
                    return ast::int_type();
                case ast::Oper::NE:
                case ast::Oper::AND:
                case ast::Oper::OR:
                    return ast::bool_type();
            }
            throw std::runtime_error("unknown operator");
        };
        auto return_type = [] (ast::Oper::OperName op)
        {
            switch (op)
            {
                case ast::Oper::PLUS:
                case ast::Oper::MINUS:
                case ast::Oper::MULT:
                case ast::Oper::DIV:
                case ast::Oper::MOD:
                    return ast::int_type();
                case ast::Oper::GT:
                case ast::Oper::LT:
                case ast::Oper::EQ:
                case ast::Oper::GE:
                case ast::Oper::LE:
                case ast::Oper::NE:
                case ast::Oper::AND:
                case ast::Oper::OR:
                    return ast::bool_type();
            }
            throw std::runtime_error("unknown operator");
        };

        auto exp_type = expected_arg_type(expr.oper.oper);
        expect_type(this->get_type(*expr.lhs), exp_type,
                    "arguments of '" + ast::to_string(expr.oper)
                    + "' must have '" + ast::to_string(exp_type) + "' type");
        expect_type(this->get_type(*expr.rhs), exp_type,
                    "arguments of '" + ast::to_string(expr.oper)
                    + "' must have '" + ast::to_string(exp_type) + "' type");
        return return_type(expr.oper.oper);
    }

    ast::Type get_type(const ast::Dereference & expr) const
    {
        undefined;
    }

    ast::Type get_type(const ast::Call & expr) const
    {
        ast::Type func_type = this->get(expr.function).first;
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
