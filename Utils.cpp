#include "Utils.h"

#include <string>
#include <fstream>
#include <streambuf>

std::string read_file_as_string(const char* const& path)
{
    std::ifstream f(path);
    return std::string(
        (std::istreambuf_iterator<char>(f)),
        std::istreambuf_iterator<char>()
    );
}
