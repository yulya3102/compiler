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
        undefined;
    }

    void verify_statement(const ast::Assignment & st)
    {
        undefined;
    }

    void verify_statement(const ast::Seq & st)
    {
        verify_statement(*st.first);
        verify_statement(*st.second);
    }

    void verify_statement(const ast::If & st)
    {
        undefined;
    }

    void verify_statement(const ast::While & st)
    {
        undefined;
    }

    void verify_statement(const ast::Read & st)
    {
        undefined;
    }

    void verify_statement(const ast::Write & st)
    {
        undefined;
    }

    void verify_statement(const ast::Return & st)
    {
        if (return_type != this->get_type(*st.expr))
            throw semantic_error(st.loc, "function return type does not match return expression type");
    }

    void verify_statement(const ast::Statement & st)
    {
        return fmap([this], x, this->verify_statement(x), st.statement);
    }

    ast::Type return_type;
};
}
