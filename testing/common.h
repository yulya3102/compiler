#include <lcc/lcc.h>

#include <vector>
#include <string>

std::vector<int> test_compiled(const std::string & code, const std::vector<int> & input,
                               lcc::Optimisations opt = lcc::Optimisations::ACC,
                               int expected_retcode = 0);

std::vector<int> random_input(size_t size, int limit = std::numeric_limits<int>::max());

#define EXPECT_EQ_RESULTS(size, limit, f, g) { auto in = random_input(size, limit); EXPECT_EQ(f(in), g(in)); }
