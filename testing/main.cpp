#include <lcc/lcc.h>
#include <sem/error.h>

#include <llvm/Support/raw_ostream.h>
#include <gtest/gtest.h>
#include <sstream>

void try_compile(const std::string & code)
{
    std::stringstream in(code);
    llvm::raw_null_ostream out;
    lcc::compile(in, out, "try_compile");
}

TEST(semantic, assignment_type_mismatch)
{
    EXPECT_THROW(try_compile("int main() { int a; a = true; }"), sem::semantic_error);
    EXPECT_THROW(try_compile("int main() { _Bool a; a = 1; }"), sem::semantic_error);
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
