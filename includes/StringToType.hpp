#ifndef	STRING_TO_TYPE_HPP_
#define STRING_TO_TYPE_HPP_

#include <string>
#include <sstream>

template<typename T>
T stringToType(const std::string& input)
{
    T output;
    std::istringstream iss(input);

    iss >> output;
    return (output);
}

#endif //! STRINGTOTYPE_HPP_
