#include "common.h"

#include <lcc/lcc.h>
#include <utils/string.h>

#include <gtest/gtest.h>

#include "compiled_fact.h"

TEST(optimised, fact)
{
    std::string code = utils::to_string(testing::compiled_fact);
    auto compiled_code = [&code] (const std::vector<int> & input)
    {
        return test_compiled(code, input, lcc::Optimisations::NONE,
                             "without optimisations");
    };
    auto tco_optimised_code = [&code] (const std::vector<int> & input)
    {
        return test_compiled(code, input, lcc::Optimisations::TCO,
                             "with tail call optimisation");
    };
    auto optimised_code = [&code] (const std::vector<int> & input)
    {
        return test_compiled(code, input, lcc::Optimisations::ACC,
                             "with rewriting to accumulator and tail call optimisation");
    };

    EXPECT_EQ_RESULTS(40000, 12, compiled_code, tco_optimised_code);
    EXPECT_EQ_RESULTS(40000, 12, compiled_code, optimised_code);
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
