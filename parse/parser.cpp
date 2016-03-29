#include "parser.h"

#include "lexer.h"

namespace ast
{
Code ast_parser::parse()
{
    lexer_t lexer;
    this->lexer = &lexer;

    generated_parser p(*this);

    code.entries.clear();
    if (p.parse())
        throw std::runtime_error("Parser failed");

    return code;
}
}
