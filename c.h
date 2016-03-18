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
    Type(AtomType type);
    Type(ConstType type);
    Type(PointerType type);

    bool operator==(const Type & rhs) const;

    boost::variant<AtomType, ConstType, PointerType> type;
};

struct Const
{
    boost::variant<bool, int> constant;
};

struct Value
{
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
    boost::variant<VarDeclaration, VarDefinition, Assignment, If, While, Expression> statement;
};

struct FuncDefinition
{
    FuncDeclaration declaration;
    FuncBody body;
};

struct Definition
{
    boost::variant<VarDefinition, FuncDefinition> definition;
};

struct CodeEntry
{
    boost::variant<Declaration, Definition> entry;
};

struct Code
{
    std::list<CodeEntry> entries;
};

}
