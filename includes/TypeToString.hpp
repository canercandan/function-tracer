#ifndef TYPE_TO_STRING_HPP_
#define TYPE_TO_STRING_HPP_

#include <string>
#include <sstream>

template<typename T>
std::string typeToString(T input)
{
    std::ostringstream oss;

    oss << input;
    return (oss.str());
}

#endif
