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
}
