#pragma once

#include <boost/variant.hpp>
#include <memory>
#include <cstdint>
#include <list>

namespace ast
{
class location;

/* Expressions */

struct Expression;

struct Const
{
    template <typename T>
    Const(const T & t)
        : constant(t)
    {}

    std::shared_ptr<location> loc;
    boost::variant<bool, std::int64_t> constant;
};

struct Value
{
    template <typename T>
    Value(const T & t)
        : value(t)
    {}

    std::shared_ptr<location> loc;
    boost::variant<
        Const,          // bool or int
        std::string     // variable name
    > value;
};

struct Call
{
    std::shared_ptr<location> loc;
    std::string function;
    std::list<Expression> arguments;
};

struct Oper
{
    enum OperName
    {
        PLUS,
        MINUS,
        MULT,
        DIV,
        MOD,
        GT,
        LT,
        EQ,
        GE,
        LE,
        NE,
        AND,
        OR
    };
    std::shared_ptr<location> loc;
    OperName oper;
};

struct BinOperator
{
    std::shared_ptr<location> loc;
    std::shared_ptr<Expression> lhs, rhs;
    Oper oper;
};

struct Dereference
{
    std::shared_ptr<location> loc;
    std::shared_ptr<Expression> expr;
};

struct Expression
{
    template <typename T>
    Expression(const T & t)
        : expression(t)
    {}

    std::shared_ptr<location> loc;
    boost::variant<Value, BinOperator, Dereference, Call> expression;
};
}
