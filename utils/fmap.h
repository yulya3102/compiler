#pragma once

#include <boost/variant.hpp>

#define fmap(closure, x, expr, variant) boost::apply_visitor(closure (const auto & x) { return expr; }, variant)
