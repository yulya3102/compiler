#pragma once

#include <parse/location.hh>

#include <stdexcept>
#include <memory>
#include <string>
#include <sstream>

namespace sem
{
struct semantic_error : std::runtime_error
{
    semantic_error(const std::shared_ptr<ast::location> & loc,
                   const std::string & what_arg)
        : std::runtime_error(what_arg)
        , loc(loc)
    {}

    const char * what() const noexcept
    {
        std::stringstream ss;
        ss << *loc << ": " << std::runtime_error::what();
        return ss.str().c_str();
    }

    std::shared_ptr<ast::location> loc;
};
}
