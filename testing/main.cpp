#include <lcc/lcc.h>
#include <sem/error.h>
#include <utils/undefined.h>

#include <llvm/Support/raw_ostream.h>
#include <gtest/gtest.h>
#include <boost/utility/string_ref.hpp>
#include <sstream>
#include <chrono>

std::string to_string(const boost::string_ref & ref)
{
    return std::string(ref.begin(), ref.end());
}

void try_compile(const std::string & code)
{
    std::stringstream in(code);
    llvm::raw_null_ostream out;
    lcc::compile_llvm(in, out, "try_compile");
}

#define EXPECT_SEMANTIC_ERROR(code) EXPECT_THROW(try_compile(code), sem::semantic_error)

TEST(semantic, assignment_type_mismatch)
{
    EXPECT_SEMANTIC_ERROR("int main() { int a; a = true; }");
    EXPECT_SEMANTIC_ERROR("int main() { _Bool a; a = 1; }");
}

std::vector<int> test_compiled(const std::string & code, const std::vector<int> & input, int expected_retcode = 0)
{
    std::stringstream in(code);
    std::string compiled = lcc::create_temp_file("test_compiled_XXXXXX");
    lcc::compile_executable(in, compiled);

    // TODO: error handling
    int process_input[2];
    pipe(process_input);
    int process_output[2];
    pipe(process_output);
    pid_t child = fork();
    if (child == 0)
    {
        dup2(process_input[0], STDIN_FILENO);
        close(process_input[0]);
        close(process_input[1]);
        dup2(process_output[1], STDOUT_FILENO);
        close(process_output[0]);
        close(process_output[1]);

        execl(compiled.c_str(), compiled.c_str(), nullptr);
    }
    close(process_input[0]);
    close(process_output[1]);

    FILE * pin = fdopen(process_input[1], "w");
    for (auto i : input)
        fprintf(pin, "%d\n", i);
    fflush(pin);
    fclose(pin);

    FILE * pout = fdopen(process_output[0], "r");
    int i;
    std::vector<int> result;
    while (fscanf(pout, "%d", &i) != EOF)
        result.push_back(i);

    int status;
    waitpid(child, &status, 0);
    if (WIFEXITED(status) && (WEXITSTATUS(status) != expected_retcode))
        throw std::runtime_error("Unexpected return code of a program: "
                                 + std::to_string(WEXITSTATUS(status)));
    if (WIFSIGNALED(status))
        throw std::runtime_error("Program was terminated by a signal "
                                 + std::to_string(WTERMSIG(status)));

    return result;
}

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

#include "compiled_fact.h"

TEST(compiled, fact)
{
    std::string code = to_string(testing::compiled_fact);
    std::vector<int> input, expected_output;
    for (std::size_t i = 1; i <= 10; ++i)
    {
        input.push_back(i);
        expected_output.push_back(fact(i));
    }
    EXPECT_EQ(test_compiled(code, input), expected_output);
}

#include "compiled_fact_accum.h"

TEST(compiled, fact_accum)
{
    std::string code = to_string(testing::compiled_fact);
    std::string code_accum = to_string(testing::compiled_fact_accum);

    std::mt19937 generator;
    std::uniform_int_distribution<std::uint64_t> distribution(1, 12);

    std::size_t size = 40000;
    std::vector<int> input;
    for (std::size_t i = 0; i < size; ++i)
        input.push_back(distribution(generator));

    auto start = std::chrono::system_clock::now();
    auto code_output = test_compiled(code, input);
    auto end = std::chrono::system_clock::now();
    auto code_duration = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(end - start);

    start = std::chrono::system_clock::now();
    auto code_accum_output = test_compiled(code_accum, input);
    end = std::chrono::system_clock::now();
    auto code_accum_duration = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(end - start);

    EXPECT_EQ(code_output, code_accum_output);
    EXPECT_LT(code_duration, code_accum_duration);
}

#include "compiled_scope.h"

TEST(compiled, scope)
{
    std::string code = to_string(testing::compiled_scope);
    std::vector<int> expected_output = {0, 88, 42, 88};
    EXPECT_EQ(test_compiled(code, {}), expected_output);
}

#include "semantic_subscope.h"

TEST(semantic, subscope)
{
    std::string code = to_string(testing::semantic_subscope);
    EXPECT_NO_THROW(try_compile(code));
}

#include "compiled_func_pointer.h"

TEST(compiled, func_pointer)
{
    std::string code = to_string(testing::compiled_func_pointer);

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
    std::string code = to_string(testing::compiled_pointer);
    std::vector<int> expected_output{0, 42};
    EXPECT_EQ(expected_output, test_compiled(code, {}));
}

#include "compiled_pointer_pointer.h"

TEST(compiled, pointer_pointer)
{
    std::string code = to_string(testing::compiled_pointer_pointer);
    std::vector<int> expected_output{0, 42};
    EXPECT_EQ(expected_output, test_compiled(code, {}));
}

#include "compiled_alloca_while.h"

TEST(compiled, alloca_while)
{
    std::string code = to_string(testing::compiled_alloca_while);
    std::vector<int> expected_output;
    for (int i = 0; i < 100000000; ++i)
        expected_output.push_back(i);

    auto res = test_compiled(code, {});

    EXPECT_EQ(expected_output, res);
}

#include "compiled_arg_var.h"

TEST(compiled, arg_var)
{
    std::string code = to_string(testing::compiled_arg_var);
    std::vector<int> expected_output = {0, 1, 0};
    EXPECT_EQ(test_compiled(code, {}), expected_output);
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
