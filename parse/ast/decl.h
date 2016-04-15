#pragma once

#include "types.h"

#include <boost/variant.hpp>
#include <memory>
#include <list>

namespace ast
{
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
}
