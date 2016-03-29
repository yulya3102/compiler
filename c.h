#pragma once

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include <list>
#include <string>
#include <memory>

namespace ast
{

/*
 * Code = [CodeEntry]
 * CodeEntry = Declaration | Definition
 * Declaration = FuncDeclaration | VarDeclaration
 * Definition = FuncDefinition | VarDefinition
 * FuncDeclaration = FuncSignature
 * VarDeclaration = VarSignature
 * FuncDefinition = FuncSignature * FuncBody
 * VarDefinition = VarSignature * Value
 * FuncSignature = Type * Name * ArgList
 * VarSignature = Type * Name
 * FuncBody = Empty | Statement * FuncBody
 * Value = Number | BoolConst
 * Type = Int | Bool | Pointer Type
 * ArgList = Empty | VarSignature
 * Statement = VarDeclaration | VarDefinition | Assignment | Call
 * Assignment = Name Value
 */

enum AtomType
{
    BOOL,
    INT
};

struct Type;

struct ConstType
{
    std::shared_ptr<Type> type;
};

struct PointerType
{
    std::shared_ptr<Type> type;
};

struct Type
{
    template <typename T>
    Type(const T & t)
        : type(t)
    {}

    boost::variant<AtomType, PointerType> type;
};

struct Const
{
    template <typename T>
    Const(const T & t)
        : constant(t)
    {}

    boost::variant<bool, int> constant;
};

struct Value
{
    template <typename T>
    Value(const T & t)
        : value(t)
    {}

    boost::variant<
        Const,          // bool or int
        std::string     // variable name
    > value;
};

struct VarDeclaration
{
    Type type;
    std::string name;
};

struct FuncDeclaration
{
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

    boost::variant<VarDeclaration, FuncDeclaration> declaration;
};

struct Expression;

struct Call
{
    std::string function;
    std::list<Expression> arguments;
};

enum Oper
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

struct BinOperator
{
    std::shared_ptr<Expression> lhs, rhs;
    Oper oper;
};

struct Expression
{
    template <typename T>
    Expression(const T & t)
        : expression(t)
    {}

    boost::variant<Value, BinOperator> expression;
};

struct VarDefinition
{
    VarDeclaration declaration;
    Expression value;
};

struct Assignment
{
    std::string varname;
    Expression value;
};

struct Statement;

struct If
{
    Expression condition;
    std::shared_ptr<Statement> thenBody, elseBody;
};

struct While
{
    Expression condition;
    std::shared_ptr<Statement> body;
};

struct Skip
{};

struct Seq
{
    std::shared_ptr<Statement> first, second;
};

struct Read
{
    std::string varname;
};

struct Write
{
    std::shared_ptr<Expression> expr;
};

struct Statement
{
    template <typename T>
    Statement(const T & t)
        : statement(t)
    {}

    boost::variant<
        Skip,
        VarDeclaration,
        Assignment,
        Seq,
        If,
        While,
        Read,
        Write>
        statement;
};

struct FuncDefinition
{
    FuncDeclaration declaration;
    Statement statement;
};

struct Definition
{
    template <typename T>
    Definition(const T & t)
        : definition(t)
    {}

    boost::variant<VarDeclaration, FuncDefinition> definition;
};

struct CodeEntry
{
    template <typename T>
    CodeEntry(const T & t)
        : entry(t)
    {}

    boost::variant<Declaration, Definition> entry;
};

struct Code
{
    std::list<CodeEntry> entries;
};

std::string to_string(AtomType);
std::string to_string(const Type &);
std::string to_string(const PointerType &);
std::string to_string(const Const &);
std::string to_string(const Value &);
std::string to_string(const VarDeclaration &);
std::string to_string(const FuncDeclaration &);
std::string to_string(const Declaration &);
std::string to_string(const Expression &);
std::string to_string(Oper);
std::string to_string(const BinOperator &);
std::string to_string(const Assignment &);
std::string to_string(const Statement &);
std::string to_string(const If &);
std::string to_string(const While &);
std::string to_string(const Skip &);
std::string to_string(const Seq &);
std::string to_string(const Read &);
std::string to_string(const Write &);
std::string to_string(const FuncDefinition &);
std::string to_string(const Definition &);
std::string to_string(const CodeEntry &);
std::string to_string(const Code &);
}
