#include <lcc/lcc.h>

#include <vector>
#include <string>

std::vector<int> test_compiled(const std::string & code, const std::vector<int> & input,
                               lcc::Optimisations opt = lcc::Optimisations::ACC,
                               const char * log_message = nullptr,
                               int expected_retcode = 0);

std::vector<int> random_input(size_t size, int limit = std::numeric_limits<int>::max());

#define EXPECT_EQ_RESULTS(size, limit, f, g) { auto in = random_input(size, limit); EXPECT_EQ(f(in), g(in)); }

namespace testing
{
struct nonzero_retcode : std::runtime_error
{
    nonzero_retcode(int retcode)
        : std::runtime_error((std::string("Unexpected return code ")
                            + std::to_string(retcode)).c_str())
    {}
};

struct killed_by_signal : std::runtime_error
{
    killed_by_signal(int signal)
        : std::runtime_error((std::string("Program was terminated by signal ")
                            + std::to_string(signal)).c_str())
    {}
};
}
