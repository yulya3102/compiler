#include "types.h"
#include "error.h"

#include <utils/undefined.h>

void sem::expect_type(const ast::Type & real, const ast::Type & expected, const std::string & error_message)
{
    if (real != expected)
        throw semantic_error(real.loc, error_message);
}


ast::Type ast::pointer_type(const ast::Type & type, std::shared_ptr<ast::location> loc)
{
    return PointerType{loc, std::shared_ptr<Type>(new Type(type))};
}

ast::Type sem::binop_expected_argtype(ast::Oper::OperName op)
{
    switch (op)
    {
        case ast::Oper::PLUS:
        case ast::Oper::MINUS:
        case ast::Oper::MULT:
        case ast::Oper::DIV:
        case ast::Oper::MOD:
        case ast::Oper::GT:
        case ast::Oper::LT:
        case ast::Oper::EQ:
        case ast::Oper::GE:
        case ast::Oper::LE:
            return ast::int_type();
        case ast::Oper::NE:
        case ast::Oper::AND:
        case ast::Oper::OR:
            return ast::bool_type();
    }
    throw std::runtime_error("unknown operator");
}


ast::Type sem::binop_rettype(ast::Oper::OperName op)
{
    switch (op)
    {
        case ast::Oper::PLUS:
        case ast::Oper::MINUS:
        case ast::Oper::MULT:
        case ast::Oper::DIV:
        case ast::Oper::MOD:
            return ast::int_type();
        case ast::Oper::GT:
        case ast::Oper::LT:
        case ast::Oper::EQ:
        case ast::Oper::GE:
        case ast::Oper::LE:
        case ast::Oper::NE:
        case ast::Oper::AND:
        case ast::Oper::OR:
            return ast::bool_type();
    }
    throw std::runtime_error("unknown operator");
}
