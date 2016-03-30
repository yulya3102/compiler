#include "parser.h"

#include "lexer.h"

namespace ast
{
Code parser::parse(std::istream & in, std::ostream & out)
{
    lexer_t lexer(in, out);
    this->lexer = &lexer;

    generated_parser p(*this);

    code.entries.clear();
    if (p.parse())
        throw std::runtime_error("Parser failed");

    return code;
}
}
