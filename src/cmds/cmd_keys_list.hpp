#pragma once

#include <system_error>
#include <filesystem>
#include <iterator>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <algorithm>

#include "types.hpp"

namespace lmail
{

class CmdKeysList
{
public:
    explicit CmdKeysList(std::filesystem::path profile_path) : profile_path_(std::move(profile_path))
    {
        if (profile_path_.empty())
            throw std::invalid_argument("profile path provided cannot be empty");
    }

    void operator()()
    try
    {
        int key_id = 0;
        std::error_code ec;
        for (auto const &item_path : std::filesystem::directory_iterator(profile_path_ / kKeysDir, ec))
        {
            if (item_path.is_directory())
            {
                if (auto const key_name = item_path.path().filename().string();
                    !key_name.empty() && '.' == key_name.front())
                {
                    std::cout << (++key_id) << ". ";
                    std::copy(key_name.cbegin() + 1, key_name.cend(), std::ostream_iterator<char>(std::cout));
                    std::cout << std::endl;
                }
            }
        }

        if (0 == key_id)
            std::cout << "There are no keys available" << std::endl;
    }
    catch (std::exception const &ex)
    {
        std::cerr << "error occurred: " << ex.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "unknown exception\n";
    }

private:
    static constexpr char kKeysDir[] = "keys";

private:
    std::filesystem::path profile_path_;
};

} // namespace lmail
