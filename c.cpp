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

Type::Type(AtomType type)
    : type(type)
{}

Type::Type(ConstType type)
    : type(type)
{}

Type::Type(PointerType type)
    : type(type)
{}

bool Type::operator==(const Type & rhs) const
{
    return type == rhs.type;
}
}
