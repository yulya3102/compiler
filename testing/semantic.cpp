#include <lcc/lcc.h>
#include <sem/error.h>
#include <utils/string.h>

#include <gtest/gtest.h>

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

#include "semantic_subscope.h"

TEST(semantic, subscope)
{
    std::string code = utils::to_string(testing::semantic_subscope);
    EXPECT_NO_THROW(try_compile(code));
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
