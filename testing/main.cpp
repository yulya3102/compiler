#include <lcc/lcc.h>
#include <sem/error.h>
#include <utils/undefined.h>

#include <llvm/Support/raw_ostream.h>
#include <gtest/gtest.h>
#include <sstream>

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

TEST(compiled, fact)
{
    std::string code =
        "int fact(int n);"
        ""
        "int main()"
        "{"
        "    int n;"
        "    int res;"
        "    while (read(n))"
        "    {"
        "        res = fact(n);"
        "        write(res);"
        "    }"
        "    return 0;"
        "}"
        ""
        "int fact(int n)"
        "{"
        "    if (n < 2)"
        "        return 1;"
        "    else {}"
        "    return n * fact(n - 1);"
        "}";
    std::vector<int> input, expected_output;
    for (std::size_t i = 1; i <= 10; ++i)
    {
        input.push_back(i);
        expected_output.push_back(fact(i));
    }
    EXPECT_EQ(test_compiled(code, input), expected_output);
}

TEST(compiled, scope)
{
    std::string code =
        "int a;"
        ""
        "int initA(int n)"
        "{"
        "   a = n;"
        "   int b;  b = 14;"
        "   return 0;"
        "}"
        ""
        "int main()"
        "{"
        "   write(a);"          // prints "0"
        "   int b;  b = 88;"
        "   write(b);"          // prints "88"
        "   int ret;"
        "   ret = initA(42);"   // function should use 'a' from outer scope and 'b' from its own scope
        "   write(a);"          // prints "42"
        "   write(b);"          // prints "88"
        ""
        "   return 0;"
        "}";
    std::vector<int> expected_output = {0, 88, 42, 88};
    EXPECT_EQ(test_compiled(code, {}), expected_output);
}

TEST(semantic, subscope)
{
    std::string code =
        "int main()"
        "{"
        // Test inner block scoping
        "   {"
        "       int b;  b = 0;"
        "   }"
        "   _Bool b; b = true;"
        ""
        // Test if scoping
        "   if (true)"
        "   {"
        "       int a; a = 1;"
        "   }"
        "   else"
        "       int c;"
        ""
        "   _Bool a;"
        "   _Bool c;"
        ""
        // Test while scoping
        "   while (true)"
        "   {"
        "       int d; d = 1;"
        "   }"
        "   while (false)"
        "       int e;"
        ""
        "   _Bool d;"
        "   _Bool e;"
        ""
        "   return 0;"
        "}";
    EXPECT_NO_THROW(try_compile(code));
}

TEST(compiled, func_pointer)
{
    std::string code = R"(
int fact(int n)
{
    if (n > 1)
        return n * fact(n - 1);
    else
        return 1;
}

int fib(int n)
{
    int prev;   prev = 1;
    int cur;    cur = 1;
    int i;      i = 2;
    int next;
    while (i < n)
    {
        next = prev + cur;
        prev = cur;
        cur = next;
        i = i + 1;
    }
    return cur;
}

int test(int start, int end, int(int) * func)
{
    int i;  i = start;
    while (i <= end)
    {
        write(i);
        write((*func)(i));
        i = i + 1;
    }
    return i;
}

int main()
{
    int x;
    x = test(1, 10, &fact);
    x = test(1, 10, &fib);
    return 0;
}
    )";

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

TEST(compiled, pointer)
{
    std::string code = R"(
int a(int * b)
{
    *b = 42;
    return 0;
}

int main()
{
    int c;
    c = 0;
    write(c);   // prints "0"
    int g;
    g = a(&c);
    write(c);   // prints "42"
    return 0;
}
    )";
    std::vector<int> expected_output{0, 42};
    EXPECT_EQ(expected_output, test_compiled(code, {}));
}

TEST(compiled, pointer_pointer)
{
    std::string code = R"(
int a(int ** b)
{
    **b = 42;
    return 0;
}

int main()
{
    int c;
    c = 0;
    write(c);   // prints "0"
    int * e;
    e = &c;
    int g;
    g = a(&e);
    write(c);   // prints "42"
    return 0;
}
    )";
    std::vector<int> expected_output{0, 42};
    EXPECT_EQ(expected_output, test_compiled(code, {}));
}

TEST(compiled, alloca_while)
{
    std::string code = R"(
int main()
{
    int i; i = 0;
    while (i < 100000000)
    {
        int j;  j = i;
        write(i);

        i = i + 1;
    }

    return 0;
}
    )";
    std::vector<int> expected_output;
    for (int i = 0; i < 100000000; ++i)
        expected_output.push_back(i);

    auto res = test_compiled(code, {});

    EXPECT_EQ(expected_output, res);
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
