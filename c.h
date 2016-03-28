#pragma once

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include <list>
#include <string>

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
    ConstType(Type type);

    bool operator==(const ConstType & rhs) const;

    std::shared_ptr<Type> type;
};

struct PointerType
{
    PointerType(Type type);

    bool operator==(const PointerType & rhs) const;

    std::shared_ptr<Type> type;
};

struct Type
{
    template <typename T>
    Type(const T & t)
        : type(t)
    {}


    bool operator==(const Type & rhs) const;

    boost::variant<AtomType, ConstType, PointerType> type;
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

enum BinOperator
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

struct Expression
{
    template <typename T>
    Expression(const T & t)
        : expression(t)
    {}

    boost::variant<Call, BinOperator, Value> expression;
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

struct FuncBody
{
    std::list<Statement> statements;
};

struct If
{
    Expression condition;
    FuncBody thenBody;
    boost::optional<FuncBody> elseBody;
};

struct While
{
    Expression condition;
    FuncBody body;
};

struct Statement
{
    template <typename T>
    Statement(const T & t)
        : statement(t)
    {}

    boost::variant<VarDeclaration, VarDefinition, Assignment, If, While, Expression> statement;
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

    boost::variant<VarDefinition, FuncDefinition> definition;
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

}
