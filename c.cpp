#include "c.h"

namespace ast
{
std::string to_string(const std::string & str)
{
    return str;
}

std::string to_string(AtomType atom_type)
{
    switch (atom_type)
    {
        case BOOL:
            return "_Bool";
        case INT:
            return "int";
    }
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

std::string to_string(Oper oper)
{
    switch (oper)
    {
        case PLUS:
            return "+";
        case MINUS:
            return "-";
        case MULT:
            return "*";
        case DIV:
            return "/";
        case MOD:
            return "%";
        case GT:
            return ">";
        case LT:
            return "<";
        case EQ:
            return "==";
        case GE:
            return ">=";
        case LE:
            return "<=";
        case NE:
            return "!=";
        case AND:
            return "&&";
        case OR:
            return "||";
    }
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
    return as.varname + " = " + to_string(as.value);
}

std::string to_string(const Statement & st)
{
    return boost::apply_visitor([] (auto & x) { return to_string(x); }, st.statement);
}

std::string to_string(const If & st)
{
    return "if (" + to_string(st.condition) + ")\n"
        + "{\n"
        + to_string(*st.thenBody);
        + "\n}\nelse {\n"
        + to_string(*st.elseBody);
        + "\n}\n";
}
std::string to_string(const While & st)
{
    return "while (" + to_string(st.condition) + ")\n"
        + "{\n"
        + to_string(*st.body);
        + "\n}\n";
}

std::string to_string(const Skip &)
{
    return "";
}

std::string to_string(const Seq & seq)
{
    return to_string(*seq.first) + ";\n" + to_string(*seq.second);
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
    return to_string(def.declaration)
        + "\n{\n"
        + to_string(def.statement)
        + "\n}\n";
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
}
