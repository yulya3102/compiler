#include <parse/ast/l.h>

namespace sem
{
ast::Type type(const ast::FuncDeclaration & entry);
ast::Type type(const ast::Expression & entry);
void verify(const ast::Code & code);
}
