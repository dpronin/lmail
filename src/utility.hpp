#pragma once

#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <system_error>
#include <utility>

#include "types.hpp"

namespace lmail
{

template <typename UnaryFunction>
void for_each_dir_entry(std::filesystem::path const &dir, UnaryFunction f)
{
    std::error_code ec;
    for (auto const &dir_entry : std::filesystem::directory_iterator(dir, ec))
        f(dir_entry);
}

template <typename Comp, typename UnaryFunction>
void for_each_dir_entry_if(std::filesystem::path const &dir, Comp comp, UnaryFunction f)
{
    std::error_code ec;
    for (auto const &dir_entry : std::filesystem::directory_iterator(dir, ec))
    {
        if (comp(dir_entry))
            f(dir_entry);
    }
}

template <typename Comp>
auto find_dir_entry_if(std::filesystem::path const &dir, Comp comp)
{
    namespace fs = std::filesystem;
    fs::directory_entry dir_entry;
    std::error_code     ec;
    auto const          rng          = fs::directory_iterator(dir, ec);
    auto                dir_entry_it = std::find_if(fs::begin(rng), fs::end(rng), comp);
    if (fs::end(rng) != dir_entry_it)
        dir_entry = *dir_entry_it;
    return dir_entry;
}

template <typename T>
T const &username_to_keyname(T const &username) { return username; }

inline auto find_key(std::filesystem::path const &dir, std::string_view keyname)
{
    // clang-format off
    return find_dir_entry_if(dir, [keyname](auto const &dir_entry) {
        return dir_entry.path().filename().stem() == keyname;
    }).path();
    // clang-format on
}

inline bool create_dir_if_doesnt_exist(std::filesystem::path const &dir)
{
    namespace fs = std::filesystem;
    if ((fs::exists(dir) && !fs::is_directory(dir)) || (!fs::exists(dir) && !fs::create_directory(dir)))
    {
        std::cerr << "couldn't create directory " << dir << '\n';
        return false;
    }
    return true;
}

} // namespace lmail
