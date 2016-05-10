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
        return test_compiled(code, input, lcc::Optimisations::NONE);
    };
    auto tco_optimised_code = [&code] (const std::vector<int> & input)
    {
        return test_compiled(code, input, lcc::Optimisations::TCO);
    };
    auto optimised_code = [&code] (const std::vector<int> & input)
    {
        return test_compiled(code, input, lcc::Optimisations::ACC);
    };

    EXPECT_EQ_RESULTS(10000, 12, tco_optimised_code, compiled_code);
    EXPECT_EQ_RESULTS(10000, 12, optimised_code, compiled_code);
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
