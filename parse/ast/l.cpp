#include "l.h"

#include <utils/undefined.h>

namespace ast
{
bool operator==(const AtomType & lhs, const AtomType & rhs)
{
    return lhs.type == rhs.type;
}

bool operator==(const FuncType & lhs, const FuncType & rhs)
{
    return (*lhs.rettype == *rhs.rettype)
        && (lhs.argtypes == rhs.argtypes);
}

bool operator==(const PointerType & lhs, const PointerType & rhs)
{
    return *lhs.type == *rhs.type;
}

bool operator==(const Type & lhs, const Type & rhs)
{
    return lhs.type == rhs.type;
}

bool operator!=(const Type & lhs, const Type & rhs)
{
    return !(lhs == rhs);
}

std::string to_string(const std::string & str)
{
    return str;
}

std::string to_string(AtomType atom_type)
{
    switch (atom_type.type)
    {
        case AtomType::BOOL:
            return "_Bool";
        case AtomType::INT:
            return "int";
    }
    throw std::runtime_error("unknown type");
}

std::string to_string(const Type & type)
{
    return boost::apply_visitor([] (auto & x) { return to_string(x); }, type.type);
}

std::string to_string(const PointerType & type)
{
    return "(" + to_string(*type.type) + ") *";
}

std::string to_string(const Const & constant)
{
    return boost::apply_visitor([] (const auto & x) { using std::to_string; return to_string(x); }, constant.constant);
}

std::string to_string(const Value & value)
{
    return boost::apply_visitor([] (auto & x) { return to_string(x); }, value.value);
}

std::string to_string(const VarDeclaration & decl)
{
    return to_string(decl.type) + " " + decl.name;
}

template <typename Iterator>
std::string to_string(Iterator begin, Iterator end, const std::string & separator = "")
{
    if (begin == end)
        return "";

    Iterator head = begin;
    begin++;
    return to_string(*head) + separator + to_string(begin, end, separator);
}

std::string to_string(const FuncType & type)
{
    std::string arglist = to_string(type.argtypes.begin(), type.argtypes.end(), ", ");
    return to_string(*type.rettype) + "(" + arglist + ")";
}

std::string to_string(const FuncDeclaration & decl)
{
    return to_string(decl.type) + " " + decl.name
        + "("
        + to_string(decl.arguments.cbegin(), decl.arguments.cend(), ", ")
        + ")";
}

std::string to_string(const Declaration & decl)
{
    return boost::apply_visitor([] (auto & x) { return to_string(x); }, decl.declaration);
}

std::string to_string(const Expression & expr)
{
    return boost::apply_visitor([] (auto & x) { return to_string(x); }, expr.expression);
}

std::string to_string(const Call & call)
{
    return to_string(*call.function)
        + "("
        + to_string(call.arguments.begin(), call.arguments.end(), ",")
        + ")";
}

std::string to_string(Oper oper)
{
    switch (oper.oper)
    {
        case Oper::PLUS:
            return "+";
        case Oper::MINUS:
            return "-";
        case Oper::MULT:
            return "*";
        case Oper::DIV:
            return "/";
        case Oper::MOD:
            return "%";
        case Oper::GT:
            return ">";
        case Oper::LT:
            return "<";
        case Oper::EQ:
            return "==";
        case Oper::GE:
            return ">=";
        case Oper::LE:
            return "<=";
        case Oper::NE:
            return "!=";
        case Oper::AND:
            return "&&";
        case Oper::OR:
            return "||";
    }
    throw std::runtime_error("unknown operator");
}

std::string to_string(const BinOperator & op)
{
    return to_string(*op.lhs) + to_string(op.oper) + to_string(*op.rhs);
}

std::string to_string(const Dereference & d)
{
    return "*(" + to_string(*d.expr) + ")";
}

std::string to_string(const Assignment & as)
{
    return to_string(as.lvalue) + " = " + to_string(as.rvalue);
}

std::string to_string(const Statement & st)
{
    return boost::apply_visitor([] (auto & x) { return to_string(x); }, st.statement);
}

std::string to_string(const If & st)
{
    return "if (" + to_string(st.condition) + ")\n"
        + to_string(*st.thenBody)
        + "\nelse\n"
        + to_string(*st.elseBody)
        + "\n";
}
std::string to_string(const While & st)
{
    return "while (" + to_string(st.condition) + ")\n"
        + to_string(*st.body)
        + "\n";
}

std::string to_string(const Block & block)
{
    return "{\n" + to_string(block.statements.begin(), block.statements.end(), ";\n") + "}";
}

std::string to_string(const Read & read)
{
    return "read(" + read.varname + ")";
}

std::string to_string(const Write & write)
{
    return "write(" + to_string(*write.expr) + ")";
}

std::string to_string(const Return & ret)
{
    return "return " + to_string(*ret.expr);
}

std::string to_string(const FuncDefinition & def)
{
    return to_string(def.declaration) + to_string(def.statements);
}

std::string to_string(const Definition & def)
{
    return boost::apply_visitor([] (auto & x) { return to_string(x); }, def.definition);
}

std::string to_string(const CodeEntry & entry)
{
    return boost::apply_visitor([] (auto & x) { return to_string(x); }, entry.entry);
}

std::string to_string(const Code & code)
{
    return to_string(code.entries.cbegin(), code.entries.cend(), "\n\n");
}

Type int_type(std::shared_ptr<location> loc)
{
    return AtomType{loc, AtomType::INT};
}

Type bool_type(std::shared_ptr<location> loc)
{
    return AtomType{loc, AtomType::BOOL};
}

std::string to_string(const Address & d)
{
    return "&" + to_string(*d.expr);
}

}
