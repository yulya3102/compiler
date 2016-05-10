#include <lcc/lcc.h>
#include <sem/error.h>
#include <utils/undefined.h>

#include <llvm/Support/raw_ostream.h>
#include <gtest/gtest.h>
#include <boost/utility/string_ref.hpp>
#include <sstream>
#include <chrono>
#include <queue>

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

struct p2open
{
    p2open(const char * command, int expected_exit_code = 0)
    : expected_exit_code(expected_exit_code)
    {
        // TODO: error handling
        int process_input[2];
        pipe(process_input);
        int process_output[2];
        pipe(process_output);
        child = fork();
        if (child == 0)
        {
            dup2(process_input[0], STDIN_FILENO);
            close(process_input[0]);
            close(process_input[1]);
            dup2(process_output[1], STDOUT_FILENO);
            close(process_output[0]);
            close(process_output[1]);

            execl(command, command, nullptr);
        }
        close(process_input[0]);
        close(process_output[1]);

        inputfd = process_input[1];
        outputfd = process_output[0];
    }

    void write(int i)
    {
        write_buffer.push(i);
    }

    void full_write(int fd, char * buf, size_t count)
    {
        while (count > 0)
        {
            ssize_t r = ::write(fd, buf, count);
            if (r == -1)
                throw std::runtime_error(strerror(errno));

            count -= r;
            buf += r;
        }
    }

    std::vector<int> read()
    {
        const size_t size = 4096;
        char buffer[size];
        while (!write_buffer.empty())
        {
            int r = snprintf(buffer, size, "%d\n", write_buffer.front());
            full_write(inputfd, buffer, r);
            write_buffer.pop();
        }
        close(inputfd);

        std::stringstream output;
        int r;
        do
        {
            r = ::read(outputfd, buffer, size);
            if (r == -1)
                throw std::runtime_error(strerror(errno));

            output << std::string(buffer, r);
        }
        while (r != 0);

        std::vector<int> result;
        while (true)
        {
            int i;
            output >> i;
            if (!output)
                break;
            result.push_back(i);
        }

        return result;
    }

    ~p2open()
    {
        int status;
        waitpid(child, &status, 0);
        if (WIFEXITED(status) && (WEXITSTATUS(status) != expected_exit_code))
            throw std::runtime_error("Unexpected return code of a program: "
                                     + std::to_string(WEXITSTATUS(status)));
        if (WIFSIGNALED(status))
            throw std::runtime_error("Program was terminated by a signal "
                                     + std::to_string(WTERMSIG(status)));
    }

    int expected_exit_code;
    pid_t child;
    int inputfd, outputfd;
    std::queue<int> write_buffer;
};

std::vector<int> test_compiled(const std::string & code, const std::vector<int> & input, int expected_retcode = 0)
{
    std::stringstream in(code);
    std::string compiled = lcc::create_temp_file("test_compiled_XXXXXX");
    lcc::compile_executable(in, compiled);

    p2open proc(compiled.c_str());
    for (auto i : input)
        proc.write(i);
    return proc.read();
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

#include "compiled_fib.h"

TEST(compiled, fib)
{
    std::string code = to_string(testing::compiled_fib);
    std::vector<int> input, expected_output;
    for (std::size_t i = 1; i <= 10; ++i)
    {
        input.push_back(i);
        expected_output.push_back(fib(i));
    }
    EXPECT_EQ(test_compiled(code, input), expected_output);
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
