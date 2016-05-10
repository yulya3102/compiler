#include "common.h"

#include <lcc/lcc.h>
#include <sem/error.h>
#include <utils/undefined.h>
#include <utils/string.h>

#include <llvm/Support/raw_ostream.h>
#include <gtest/gtest.h>
#include <sstream>
#include <chrono>
#include <queue>

int fact(int n)
{
    if (n > 1)
        return n * fact(n - 1);

    return 1;
}

int fib(int n)
{
    if (n > 2)
        return fib(n - 1) + fib(n - 2);

    return 1;
}

std::vector<int> random_input(size_t size, int limit = std::numeric_limits<int>::max())
{
    static std::mt19937 generator;
    std::uniform_int_distribution<int> distribution(0, limit);

    std::vector<int> result;
    for (size_t i = 0; i < size; ++i)
    {
        int random = distribution(generator);
        result.push_back(random);
    }

    return result;
}

#define EXPECT_EQ_RESULTS(size, limit, f, g) { auto in = random_input(size, limit); EXPECT_EQ(f(in), g(in)); }

#include "compiled_fact.h"

TEST(compiled, fact)
{
    std::string code = utils::to_string(testing::compiled_fact);
    auto compiled_code = [&code] (const std::vector<int> & input)
    {
        return test_compiled(code, input);
    };
    auto expected = [] (const std::vector<int> & input)
    {
        std::vector<int> output;
        std::transform(input.begin(), input.end(), std::back_inserter(output), fact);
        return output;
    };

    EXPECT_EQ_RESULTS(10000, 12, compiled_code, expected);
}

#include "compiled_scope.h"

TEST(compiled, scope)
{
    std::string code = utils::to_string(testing::compiled_scope);
    std::vector<int> expected_output = {0, 88, 42, 88};
    EXPECT_EQ(test_compiled(code, {}), expected_output);
}

#include "compiled_func_pointer.h"

TEST(compiled, func_pointer)
{
    std::string code = utils::to_string(testing::compiled_func_pointer);

    std::vector<int> expected_output;
    for (std::size_t i = 1; i <= 10; ++i)
    {
        expected_output.push_back(i);
        expected_output.push_back(fact(i));
    }
    for (std::size_t i = 1; i <= 10; ++i)
    {
        expected_output.push_back(i);
        expected_output.push_back(fib(i));
    }
    EXPECT_EQ(expected_output, test_compiled(code, {}));
}

#include "compiled_pointer.h"

TEST(compiled, pointer)
{
    std::string code = utils::to_string(testing::compiled_pointer);
    std::vector<int> expected_output{0, 42};
    EXPECT_EQ(expected_output, test_compiled(code, {}));
}

#include "compiled_pointer_pointer.h"

TEST(compiled, pointer_pointer)
{
    std::string code = utils::to_string(testing::compiled_pointer_pointer);
    std::vector<int> expected_output{0, 42};
    EXPECT_EQ(expected_output, test_compiled(code, {}));
}

#include "compiled_alloca_while.h"

TEST(compiled, alloca_while)
{
    std::string code = utils::to_string(testing::compiled_alloca_while);
    std::vector<int> expected_output;
    for (int i = 0; i < 100000000; ++i)
        expected_output.push_back(i);

    auto res = test_compiled(code, {});

    EXPECT_EQ(expected_output, res);
}

#include "compiled_arg_var.h"

TEST(compiled, arg_var)
{
    std::string code = utils::to_string(testing::compiled_arg_var);
    std::vector<int> expected_output = {0, 1, 0};
    EXPECT_EQ(test_compiled(code, {}), expected_output);
}

#include "compiled_fib.h"

TEST(compiled, fib)
{
    std::string code = utils::to_string(testing::compiled_fib);
    auto compiled_code = [&code] (const std::vector<int> & input)
    {
        return test_compiled(code, input);
    };
    auto expected = [] (const std::vector<int> & input)
    {
        std::vector<int> output;
        std::transform(input.begin(), input.end(), std::back_inserter(output), fib);
        return output;
    };

    EXPECT_EQ_RESULTS(10000, 25, compiled_code, expected);
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
