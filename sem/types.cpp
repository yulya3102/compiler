#include "types.h"
#include "error.h"

void sem::expect_type(const ast::Type & real, const ast::Type & expected, const std::string & error_message)
{
    if (real != expected)
        throw semantic_error(real.loc, error_message);
}
