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

TEST(implemented, pointers)
{
    try_compile("int main() { int a; a = 0; int * b; b = &a; write(*b); return 0; }");
}

std::string test_compiled(const std::string & code, const std::string & input)
{
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
    std::stringstream input, expected_output;
    std::function<int(int)> fact = [&fact] (int n) { if (n > 1) return n * fact(n - 1); else return 1; };
    for (std::size_t i = 1; i <= 10; ++i)
    {
        input << i << std::endl;
        expected_output << fact(i) << std::endl;
    }
    EXPECT_EQ(test_compiled(code, input.str()), expected_output.str());
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
