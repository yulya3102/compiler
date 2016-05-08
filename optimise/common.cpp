#include "l.h"

#include <utils/undefined.h>

namespace optimise
{
namespace
{
struct EntryVisitor : boost::static_visitor<void>
{
    using Optimisation = std::function<std::list<codegen::Function>(codegen::Function &)>;
    EntryVisitor(const Optimisation & optimisation)
        : optimisation(optimisation)
    {}

    void operator() (codegen::Variable &)
    {}

    void operator() (codegen::Function & f)
    {
        auto extras = optimisation(f);
        for (auto extra : extras)
            extra_functions.push_back(extra);
    }

    Optimisation optimisation;
    std::list<codegen::Function> extra_functions;
};
}

void optimise_functions(
    std::function<std::list<codegen::Function>(codegen::Function &)> optimisation,
    codegen::Code & code)
{
    EntryVisitor visitor(optimisation);
    for (codegen::CodeEntry & entry : code.entries)
        boost::apply_visitor(visitor, entry.entry);
    for (auto extra_function : visitor.extra_functions)
        code.entries.push_back(codegen::CodeEntry(extra_function));
}
}
