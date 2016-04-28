#pragma once

#include "expr.h"
#include "decl.h"

#include <boost/variant.hpp>
#include <memory>

namespace ast
{
/* Statements */

struct Assignment
{
    std::shared_ptr<location> loc;
    Expression lvalue;
    Expression rvalue;
};

struct Statement;

struct If
{
    std::shared_ptr<location> loc;
    Expression condition;
    std::shared_ptr<Statement> thenBody, elseBody;
};

struct While
{
    std::shared_ptr<location> loc;
    Expression condition;
    std::shared_ptr<Statement> body;
};

struct Read
{
    std::shared_ptr<location> loc;
    std::string varname;
};

struct Write
{
    std::shared_ptr<location> loc;
    std::shared_ptr<Expression> expr;
};

struct Return
{
    std::shared_ptr<location> loc;
    std::shared_ptr<Expression> expr;
};

struct Block
{
    std::shared_ptr<location> loc;
    std::list<Statement> statements;
};

struct Statement
{
    template <typename T>
    explicit Statement(const T & t)
        : statement(t)
    {}

    std::shared_ptr<location> loc;
    boost::variant<
        VarDeclaration,
        Assignment,
        If,
        While,
        Read,
        Write,
        Return,
        Block
        > statement;
};

}
