#include "ast_parser.h"

#include <iostream>


int main()
{
    ast::ast_parser p;
    ast::Code code = p.parse();
    std::cout << "Parsed " << code.entries.size() << " entries" << std::endl;
}
