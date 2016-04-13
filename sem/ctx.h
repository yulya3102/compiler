#pragma once

#include <boost/optional.hpp>

#include <unordered_map>

namespace sem
{
template <typename T>
struct context
{
    context(context * outer_scope = nullptr)
        : outer_scope(outer_scope)
    {}

    void declare(T t, const std::string & name)
    {
        auto it = locals.find(name);
        if (it == locals.end())
            locals.emplace(name, t);
        else if (it->second != t)
            throw std::runtime_error("symbol '" + name + "' redeclared with different value");
    }

    bool is_declared(const std::string & name)
    {
        boost::optional<T> res = maybe_get(name);
        return static_cast<bool>(res);
    }

    T get(const std::string & name) const
    {
        boost::optional<T> res = maybe_get(name);
        if (res)
            return *res;
        throw std::runtime_error("undefined symbol: " + name);
    }

private:
    boost::optional<T> maybe_get(const std::string & name) const
    {
        auto it = locals.find(name);

        if (it != locals.end())
            return it->second;

        if (outer_scope)
            return outer_scope->get(name);

        return boost::none;
    }

    std::unordered_map<std::string, T> locals;
    context * outer_scope;
};
}
