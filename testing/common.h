#include <lcc/lcc.h>

#include <vector>
#include <string>

std::vector<int> test_compiled(const std::string & code, const std::vector<int> & input,
                               lcc::Optimisations opt = lcc::Optimisations::ACC,
                               int expected_retcode = 0);
