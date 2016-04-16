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
