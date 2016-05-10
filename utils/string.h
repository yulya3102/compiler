#include <boost/utility/string_ref.hpp>

namespace utils
{
template <typename Iterator>
std::string to_string(Iterator begin, Iterator end, const std::string & separator = "")
{
    if (begin == end)
        return "";

    Iterator head = begin;
    begin++;
    return to_string(*head) + separator + to_string(begin, end, separator);
}

inline std::string to_string(const boost::string_ref & ref)
{
    return std::string(ref.begin(), ref.end());
}
}
