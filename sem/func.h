#pragma once

#include "types.h"

#include <parse/ast/types.h>

namespace sem
{
template <typename T>
struct function_ctx : sem::typed_ctx<T>
{
    ast::Type return_type;
};
}
