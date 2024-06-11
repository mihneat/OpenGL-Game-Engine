#include "Utils.h"

std::string Utils::ExtractClassName(const char* str)
{
    // Make a copy of the string
    const std::string fullClassName = std::string(str);
    const std::string delimiter = "::";

    const size_t pos = fullClassName.rfind(delimiter);
    std::string token = fullClassName.substr(pos + delimiter.length(), fullClassName.length() - pos);

    return token;
}
