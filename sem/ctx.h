#pragma once

#include <boost/optional.hpp>

#include <unordered_map>

namespace sem
{
template <typename T>
struct context
{
    context(const context * outer_scope = nullptr)
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

    bool is_locally_declared(const std::string & name) const
    {
        auto r = maybe_get_local(name);
        return static_cast<bool>(r);
    }

    bool is_declared(const std::string & name) const
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
    boost::optional<T> maybe_get_local(const std::string & name) const
    {
        auto it = locals.find(name);
        if (it != locals.end())
            return it->second;
        return boost::none;
    }

    boost::optional<T> maybe_get(const std::string & name) const
    {
        auto r = maybe_get_local(name);
        if (r)
            return *r;

        if (outer_scope)
            return outer_scope->maybe_get(name);

        return boost::none;
    }

    std::unordered_map<std::string, T> locals;
    const context * outer_scope;
};
}
