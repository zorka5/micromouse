#pragma once

#include <string>

std::string read_file_as_string(const char* const& path);

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;