#pragma once

#include "types.h"
#include "error.h"

#include <parse/ast/types.h>

#include <utils/fmap.h>
#include <utils/top.h>

namespace sem
{
struct function_ctx : typed_ctx<top>
{
    function_ctx(const ast::Type & return_type,
                 const typed_ctx<top> * outer_scope = nullptr)
        : typed_ctx<top>(outer_scope)
        , return_type(return_type)
    {}

    void verify_expr(const ast::Const & expr)
    { }

    void verify_expr(const std::string & expr)
    {
        if (!this->is_declared(expr))
            throw semantic_error(undefined_expr(std::shared_ptr<ast::location>), expr + " was not declared in this scope");
    }

    void verify_expr(const ast::Value & expr)
    {
        fmap([this], x, this->verify_expr(x), expr.value);
    }

    void verify_expr(const ast::BinOperator & expr)
    {
        this->verify_expr(*expr.lhs);
        this->verify_expr(*expr.rhs);

        auto exp_type = binop_expected_argtype(expr.oper.oper);
        for (auto type : {*expr.lhs, *expr.rhs})
            expect_type(this->get_type(type), exp_type,
                        "arguments of '" + ast::to_string(expr.oper)
                        + "' must have '" + ast::to_string(exp_type) + "' type");
    }

    void verify_expr(const ast::Dereference & expr)
    {
        this->verify_expr(*expr.expr);

        ast::Type type = this->get_type(*expr.expr);

        auto pointer_type = boost::get<ast::PointerType>(&type.type);
        if (!pointer_type)
            throw semantic_error(expr.loc, "trying to dereference non-pointer type");
    }

    void verify_expr(const ast::Address & expr)
    {
        this->verify_expr(*expr.expr);
    }

    void verify_expr(const ast::Call & expr)
    {
        this->verify_expr(*expr.function);
        for (auto arg : expr.arguments)
            this->verify_expr(arg);

        ast::Type type = this->get_type(*expr.function);

        auto func_type = boost::get<ast::FuncType>(&type.type);
        if (!func_type)
            throw semantic_error(expr.loc, "this expression cannot be used as a function");

        if (func_type->argtypes.size() != expr.arguments.size())
            throw semantic_error(expr.loc, "function called with wrong number of parameters");

        auto it = func_type->argtypes.begin();
        for (auto called_it = expr.arguments.begin(); called_it != expr.arguments.end(); ++called_it, ++it)
            expect_type(this->get_type(*called_it), *it, "argument type mismatch");
    }

    void verify_expr(const ast::Expression & expr)
    {
        fmap([this], x, this->verify_expr(x), expr.expression);
    }

    void verify_statement(const ast::VarDeclaration & st)
    {
        if (this->is_locally_declared(st.name))
            throw semantic_error(st.loc, "variable '" + st.name + "' was already declared");
        this->declare({st.type, top()}, st.name);
    }

    void verify_statement(const ast::Assignment & st)
    {
        verify_expr(st.lvalue);
        verify_expr(st.rvalue);
        auto lvalue_type = this->get_type(st.lvalue);
        auto rvalue_type = this->get_type(st.rvalue);
        expect_type(rvalue_type, lvalue_type, "location type does not match assigned expression type");
    }

    void verify_statement(const ast::If & st)
    {
        verify_expr(st.condition);
        expect_type(this->get_type(st.condition), ast::bool_type(), "condition must have boolean type");
        verify_statement(*st.thenBody);
        verify_statement(*st.elseBody);
    }

    void verify_statement(const ast::While & st)
    {
        verify_expr(st.condition);
        expect_type(this->get_type(st.condition), ast::bool_type(), "condition must have boolean type");
        verify_statement(*st.body);
    }

    void verify_statement(const ast::Read & st)
    {
        verify_expr(st.varname);
        expect_type(this->get_type(st.varname), ast::int_type(), "read() argument must have integer type");
    }

    void verify_statement(const ast::Write & st)
    {
        verify_expr(*st.expr);
        auto real = this->get_type(*st.expr);
        expect_type(real, ast::int_type(), "write() argument must have integer type");
    }

    void verify_statement(const ast::Return & st)
    {
        verify_expr(*st.expr);
        expect_type(this->get_type(*st.expr), return_type, "function return type does not match return expression type");
    }

    void verify_statement(const ast::Block & st)
    {
        undefined;
    }

    void verify_statement(const ast::Statement & st)
    {
        return fmap([this], x, this->verify_statement(x), st.statement);
    }

    ast::Type return_type;
};
}
