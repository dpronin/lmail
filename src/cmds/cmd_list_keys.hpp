#pragma once

#include <system_error>
#include <filesystem>
#include <iterator>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <vector>
#include <string>

#include <boost/algorithm/string/join.hpp>

#include "utility.hpp"
#include "application.hpp"

namespace lmail
{

class CmdListKeys
{
public:
    explicit CmdListKeys(std::filesystem::path profile_path) : profile_path_(std::move(profile_path))
    {
        if (profile_path_.empty())
            throw std::invalid_argument("profile path provided cannot be empty");
    }

    void operator()()
    try
    {
        size_t count = 0;
        for_each_dir_entry(profile_path_ / Application::kKeysDirName,
            [this, &count](auto const &dir_entry){
                auto const key_path = dir_entry.path();
                std::cout << "* " << key_path.filename().string();
                std::vector<std::string> usernames;
                for_each_dir_entry_if(profile_path_ / Application::kAssocsDirName,
                    [&key_path](auto const &dir_entry){
                        std::error_code ec;
                        return std::filesystem::read_symlink(dir_entry.path(), ec) == key_path && !ec;
                    },
                    [&usernames](auto const &dir_entry) { usernames.push_back(dir_entry.path().stem()); });
                if (!usernames.empty())
                    std::cout << " (associated with: " << boost::algorithm::join(usernames, ", ") << ')';
                std::cout << std::endl;
                ++count;
            }
        );
        if (0 == count)
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
    static constexpr char kKeysDirName[] = "keys";

private:
    std::filesystem::path profile_path_;
};

} // namespace lmail
