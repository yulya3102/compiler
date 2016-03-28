#include "c.h"

namespace ast
{
ConstType::ConstType(Type type)
    : type(new Type(type))
{}

PointerType::PointerType(Type type)
    : type(new Type(type))
{}

bool ConstType::operator==(const ConstType & rhs) const
{
    return *type == *rhs.type;
}

bool PointerType::operator==(const PointerType & rhs) const
{
    return *type == *rhs.type;
}

bool Type::operator==(const Type & rhs) const
{
    return type == rhs.type;
}

VarDeclaration::VarDeclaration(const Type & type, const std::string & name)
    : type(type)
    , name(name)
{}

FuncDeclaration::FuncDeclaration(const Type & type, const std::string & name,
                                 std::list<VarDeclaration> arguments)
    : type(type)
    , name(name)
    , arguments(arguments)
{}

FuncDefinition::FuncDefinition(const FuncDeclaration & declaration,
                               const Statement & statement)
    : declaration(declaration)
    , statement(statement)
{}
}
