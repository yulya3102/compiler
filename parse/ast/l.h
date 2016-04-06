#pragma once

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include <list>
#include <string>
#include <memory>

namespace ast
{

class location;

/* Types */
struct AtomType
{
    enum AtomTypeName
    {
        BOOL,
        INT
    };

    std::shared_ptr<location> loc;
    AtomTypeName type;
};

struct Type;

struct PointerType
{
    std::shared_ptr<location> loc;
    std::shared_ptr<Type> type;
};

struct FuncType
{
    std::shared_ptr<location> loc;
    std::shared_ptr<Type> rettype;
    std::list<Type> argtypes;
};

struct Type
{
    template <typename T>
    Type(const T & t)
        : loc(t.loc)
        , type(t)
    {}

    std::shared_ptr<location> loc;
    boost::variant<AtomType, PointerType, FuncType> type;
};

Type int_type(std::shared_ptr<location> loc = nullptr);
Type bool_type(std::shared_ptr<location> loc = nullptr);

/* Declarations */

struct VarDeclaration
{
    std::shared_ptr<location> loc;
    Type type;
    std::string name;
};

struct FuncDeclaration
{
    std::shared_ptr<location> loc;
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

    std::shared_ptr<location> loc;
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

    std::shared_ptr<location> loc;
    boost::variant<bool, int64_t> constant;
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

/* Definitions */

struct FuncDefinition
{
    std::shared_ptr<location> loc;
    FuncDeclaration declaration;
    Statement statement;
};

struct Definition
{
    template <typename T>
    Definition(const T & t)
        : definition(t)
    {}

    std::shared_ptr<location> loc;
    boost::variant<VarDeclaration, FuncDefinition> definition;
};

struct CodeEntry
{
    template <typename T>
    CodeEntry(const T & t)
        : entry(t)
    {}

    std::shared_ptr<location> loc;
    boost::variant<Declaration, Definition> entry;
};

struct Code
{
    std::shared_ptr<location> loc;
    std::list<CodeEntry> entries;
};

bool operator==(const AtomType & lhs, const AtomType & rhs);
bool operator==(const FuncType & lhs, const FuncType & rhs);
bool operator==(const PointerType & lhs, const PointerType & rhs);
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
