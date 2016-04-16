#pragma once

#include <boost/variant.hpp>
#include <memory>
#include <list>

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
Type pointer_type(const Type & type, std::shared_ptr<location> loc = nullptr);
}
