#include <parse/ast/l.h>

namespace sem
{
ast::Type type(const ast::Expression & expr);

void verify(const ast::Code & code);
}
