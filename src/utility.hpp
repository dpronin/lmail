#pragma once

#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <concepts>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <system_error>
#include <utility>

#include "types.hpp"

namespace lmail
{

void for_each_dir_entry(std::filesystem::path const& dir, std::invocable<std::filesystem::directory_entry> auto f)
{
    std::error_code ec;
    for (auto const& dir_entry : std::filesystem::directory_iterator(dir, ec))
        f(dir_entry);
}

inline void for_each_dir_entry_if(
    std::filesystem::path const& dir, std::predicate<std::filesystem::directory_entry> auto comparator, std::invocable<std::filesystem::directory_entry> auto f)
{
    std::error_code ec;
    for (auto const& dir_entry : std::filesystem::directory_iterator(dir, ec)) {
        if (comparator(dir_entry))
            f(dir_entry);
    }
}

inline std::filesystem::directory_entry find_dir_entry_if(std::filesystem::path const& dir, std::predicate<std::filesystem::directory_entry> auto comparator)
{
    namespace fs = std::filesystem;
    fs::directory_entry dir_entry;
    std::error_code ec;
    auto const rng = fs::directory_iterator(dir, ec);
    if (auto dir_entry_it = std::find_if(fs::begin(rng), fs::end(rng), comparator); fs::end(rng) != dir_entry_it)
        dir_entry = *dir_entry_it;
    return dir_entry;
}

std::string username_to_keyname(std::convertible_to<std::string> auto const& username) { return username; }

inline auto find_key(std::filesystem::path const& dir, std::string_view keyname)
{
    // clang-format off
    return find_dir_entry_if(dir, [keyname](auto const &dir_entry) {
        return dir_entry.path().filename().stem() == keyname;
    }).path();
    // clang-format on
}

inline bool create_dir_if_doesnt_exist(std::filesystem::path const& dir)
{
    namespace fs = std::filesystem;
    if ((fs::exists(dir) && !fs::is_directory(dir)) || (!fs::exists(dir) && !fs::create_directories(dir))) {
        std::cerr << "couldn't create directory " << dir << '\n';
        return false;
    }
    return true;
}

} // namespace lmail
