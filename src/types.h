
#pragma once

#include <common/util.h>
#include <common/utf8.h>
#include <utility>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <fstream>
#include <algorithm>
#include <experimental/filesystem>
#include <limits>

namespace std {
namespace filesystem = std::experimental::filesystem;
}

namespace bot {
template <class Type>
using allocator = std::allocator<Type>;
using string  = std::basic_string<char, std::char_traits<char>, allocator<char>>;
using wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, allocator<wchar_t>>;
using string_view  = std::string_view;
using wstring_view = std::wstring_view;
template <class Type>
using vector = std::vector<Type, allocator<Type>>;
namespace fs = std::filesystem;

constexpr double DoubleEps = std::numeric_limits<double>::epsilon();
} // namespace bot
