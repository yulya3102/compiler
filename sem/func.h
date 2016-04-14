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

    void verify_statement(const ast::Skip & st)
    {}

    void verify_statement(const ast::VarDeclaration & st)
    {
        if (this->is_locally_declared(st.name))
            throw semantic_error(st.loc, "variable '" + st.name + "' was already declared");
        this->declare({st.type, top()}, st.name);
    }

    void verify_statement(const ast::Assignment & st)
    {
        auto var_type = this->get_type(st.varname);
        auto expr_type = this->get_type(st.value);
        expect_type(expr_type, var_type, "variable type does not match assigned expression type");
    }

    void verify_statement(const ast::Seq & st)
    {
        verify_statement(*st.first);
        verify_statement(*st.second);
    }

    void verify_statement(const ast::If & st)
    {
        expect_type(this->get_type(st.condition), ast::bool_type(), "condition must have boolean type");
        verify_statement(*st.thenBody);
        verify_statement(*st.elseBody);
    }

    void verify_statement(const ast::While & st)
    {
        expect_type(this->get_type(st.condition), ast::bool_type(), "condition must have boolean type");
        verify_statement(*st.body);
    }

    void verify_statement(const ast::Read & st)
    {
        expect_type(this->get_type(st.varname), ast::int_type(), "read() argument must have integer type");
    }

    void verify_statement(const ast::Write & st)
    {
        expect_type(this->get_type(*st.expr), ast::int_type(), "write() argument must have integer type");
    }

    void verify_statement(const ast::Return & st)
    {
        expect_type(this->get_type(*st.expr), return_type, "function return type does not match return expression type");
    }

    void verify_statement(const ast::Statement & st)
    {
        return fmap([this], x, this->verify_statement(x), st.statement);
    }

    ast::Type return_type;
};
}
