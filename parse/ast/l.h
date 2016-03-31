#pragma once

#include "../location.hh"

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include <list>
#include <string>
#include <memory>

namespace ast
{

/* Types */
struct AtomType
{
    enum AtomTypeName
    {
        BOOL,
        INT
    };

    location loc;
    AtomTypeName type;
};

struct Type;

struct PointerType
{
    location loc;
    std::shared_ptr<Type> type;
};

struct FuncType
{
    std::shared_ptr<Type> rettype;
    std::list<Type> argtypes;
};

struct Type
{
    template <typename T>
    Type(const T & t)
        : type(t)
    {}

    location loc;
    boost::variant<AtomType, PointerType, FuncType> type;
};

/* Declarations */

struct VarDeclaration
{
    location loc;
    Type type;
    std::string name;
};

struct FuncDeclaration
{
    location loc;
    Type type;
    std::string name;
    std::list<VarDeclaration> arguments;
};

struct Declaration
{
    template <typename T>
    Declaration(const T & t)
        : declaration(t)
    {}

    location loc;
    boost::variant<FuncDeclaration> declaration;
};

/* Expressions */

struct Expression;

struct Const
{
    template <typename T>
    Const(const T & t)
        : constant(t)
    {}

    location loc;
    boost::variant<bool, int64_t> constant;
};

struct Value
{
    template <typename T>
    Value(const T & t)
        : value(t)
    {}

    location loc;
    boost::variant<
        Const,          // bool or int
        std::string     // variable name
    > value;
};

struct Call
{
    location loc;
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
    location loc;
    OperName oper;
};

struct BinOperator
{
    location loc;
    std::shared_ptr<Expression> lhs, rhs;
    Oper oper;
};

struct Dereference
{
    location loc;
    std::shared_ptr<Expression> expr;
};

struct Expression
{
    template <typename T>
    Expression(const T & t)
        : expression(t)
    {}

    location loc;
    boost::variant<Value, BinOperator, Dereference, Call> expression;
};

/* Statements */

struct VarDefinition
{
    location loc;
    VarDeclaration declaration;
    Expression value;
};

struct Assignment
{
    location loc;
    std::string varname;
    Expression value;
};

struct Statement;

struct If
{
    location loc;
    Expression condition;
    std::shared_ptr<Statement> thenBody, elseBody;
};

struct While
{
    location loc;
    Expression condition;
    std::shared_ptr<Statement> body;
};

struct Skip
{
    location loc;
};

struct Seq
{
    location loc;
    std::shared_ptr<Statement> first, second;
};

struct Read
{
    location loc;
    std::string varname;
};

struct Write
{
    location loc;
    std::shared_ptr<Expression> expr;
};

struct Return
{
    location loc;
    std::shared_ptr<Expression> expr;
};

struct Statement
{
    template <typename T>
    explicit Statement(const T & t)
        : statement(t)
    {}

    location loc;
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

/* Definitions */

struct FuncDefinition
{
    location loc;
    FuncDeclaration declaration;
    Statement statement;
};

struct Definition
{
    template <typename T>
    Definition(const T & t)
        : definition(t)
    {}

    location loc;
    boost::variant<VarDeclaration, FuncDefinition> definition;
};

struct CodeEntry
{
    template <typename T>
    CodeEntry(const T & t)
        : entry(t)
    {}

    location loc;
    boost::variant<Declaration, Definition> entry;
};

struct Code
{
    location loc;
    std::list<CodeEntry> entries;
};

bool operator==(const Type &, const Type &);
bool operator!=(const Type &, const Type &);

std::string to_string(AtomType);
std::string to_string(const Type &);
std::string to_string(const PointerType &);
std::string to_string(const FuncType &);
std::string to_string(const Const &);
std::string to_string(const Value &);
std::string to_string(const VarDeclaration &);
std::string to_string(const FuncDeclaration &);
std::string to_string(const Declaration &);
std::string to_string(const Expression &);
std::string to_string(const Call &);
std::string to_string(Oper);
std::string to_string(const BinOperator &);
std::string to_string(const Dereference & d);
std::string to_string(const Assignment &);
std::string to_string(const Statement &);
std::string to_string(const If &);
std::string to_string(const While &);
std::string to_string(const Skip &);
std::string to_string(const Seq &);
std::string to_string(const Read &);
std::string to_string(const Write &);
std::string to_string(const Return &);
std::string to_string(const FuncDefinition &);
std::string to_string(const Definition &);
std::string to_string(const CodeEntry &);
std::string to_string(const Code &);
}
