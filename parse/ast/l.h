#pragma once

#include "types.h"
#include "expr.h"
#include "decl.h"
#include "statement.h"

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include <list>
#include <string>
#include <memory>

namespace ast
{
class location;

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
std::string to_string(const Address & d);
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
