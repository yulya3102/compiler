#pragma once

#include "expr.h"
#include "decl.h"

#include <boost/variant.hpp>
#include <memory>

namespace ast
{
/* Statements */

struct VarDefinition
{
    std::shared_ptr<location> loc;
    VarDeclaration declaration;
    Expression value;
};

struct Assignment
{
    std::shared_ptr<location> loc;
    std::string varname;
    Expression value;
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

struct Skip
{
    std::shared_ptr<location> loc;
};

struct Seq
{
    std::shared_ptr<location> loc;
    std::shared_ptr<Statement> first, second;
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

struct Statement
{
    template <typename T>
    explicit Statement(const T & t)
        : statement(t)
    {}

    std::shared_ptr<location> loc;
    boost::variant<
        Skip,
        VarDeclaration,
        Assignment,
        Seq,
        If,
        While,
        Read,
        Write,
        Return
        > statement;
};

}
