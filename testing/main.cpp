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

std::vector<int> test_compiled(const std::string & code, const std::vector<int> & input)
{
    std::stringstream in(code);
    std::string compiled = lcc::create_temp_file("test_compiled_XXXXXX");
    lcc::compile_executable(in, compiled);

    undefined;
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
        "    while (true)"
        "    {"
        "        read(n);"
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
    std::function<int(int)> fact = [&fact] (int n) { if (n > 1) return n * fact(n - 1); else return 1; };
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
    std::string code = R"(
// Test function calls

int ten()
{
    return 10;
}

int a;

int initA(int n)
{
    a = n;
    int b;    b = 14;
    return 0;
}

int main()
{
    write(ten());       // prints "10"
    write(a);           // prints "0"
    int b;
    b = 88;
    write(b);           // prints "88"
    int ret;
    ret = initA(42);    // function uses 'a' from outer scope and 'b' from its own scope
    write(a);           // prints "42"
    write(b);           // prints "88"

    return 0;
})";
    std::vector<int> expected_output = {10, 0, 88, 42, 88};
    EXPECT_EQ(test_compiled(code, {}), expected_output);
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
