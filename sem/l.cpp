#include "l.h"

#include <utils/undefined.h>

#include <unordered_map>
#include <unordered_set>
#include <iostream>

namespace
{
struct context
{
    std::unordered_map<std::string, ast::Type> declared;
    std::unordered_set<std::string> defined;
    context * parent;
};
}

namespace sem
{

ast::Type type(const ast::Expression & expr)
{
    undefined;
}

ast::Type type(const ast::CodeEntry & entry)
{
    undefined;
}

std::string name(const ast::Declaration & entry)
{
    undefined;
}

std::string name(const ast::Definition & entry)
{
    undefined;
}

std::string name(const ast::CodeEntry & entry)
{
    undefined;
}

ast::location location(const ast::CodeEntry & entry)
{
    undefined;
}

bool is_definition(const ast::CodeEntry & entry)
{
    undefined;
}

bool verify(const context & ctx, const ast::CodeEntry & entry)
{
    undefined;
}

bool verify(const ast::Code & code)
{
    context ctx;
    for (auto entry : code.entries)
    {
        std::string n = name(entry);
        if (ctx.declared.find(n) != ctx.declared.end())
        {
            if (ctx.declared.at(n) != type(entry))
            {
                std::cerr << "error at " << location(entry)
                          << ": symbol '" << n
                          << "' redeclared with different type"
                          << std::endl;
                return false;
            }

            if (ctx.defined.find(n) != ctx.defined.end() && is_definition(entry))
            {
                std::cerr << "error at " << location(entry)
                          << ": symbol '" << n
                          << "' was already defined"
                          << std::endl;
                return false;
            }
        }
        else
            ctx.declared.emplace(name(entry), type(entry));

        if (!verify(ctx, entry))
            return false;

        if (is_definition(entry))
            ctx.defined.insert(name(entry));
    }
    return true;
}

}
