#pragma once

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <boost/algorithm/string/join.hpp>

#include "application.hpp"
#include "logged_user.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdListKeys final
{
public:
    explicit CmdListKeys(std::shared_ptr<LoggedUser> logged_user) : logged_user_(std::move(logged_user))
    {
        if (!logged_user_)
            throw std::invalid_argument("logged user provided cannot be empty");
    }

    void operator()()
    try
    {
        print_own();
        print_imported();
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
    void print_own() const
    {
        std::cout << "Own keys:" << std::endl;
        size_t count = 0;
        for_each_dir_entry(logged_user_->profile().keys_dir(), [this, &count](auto const &dir_entry) {
            auto const key_pair_path = dir_entry.path();
            std::cout << "* " << key_pair_path.stem().string();
            std::vector<std::string> usernames;
            for_each_dir_entry_if(
                logged_user_->profile().assocs_dir(),
                [&key_pair_path](auto const &dir_entry) {
                    std::error_code ec;
                    return std::filesystem::read_symlink(dir_entry.path(), ec) == key_pair_path && !ec;
                },
                [&usernames](auto const &dir_entry) { usernames.push_back(dir_entry.path().stem()); });
            if (!usernames.empty())
                std::cout << " (associated with: " << boost::algorithm::join(usernames, ", ") << ')';
            std::cout << std::endl;
            ++count;
        });
        if (0 == count)
            std::cout << "There are no keys available" << std::endl;
    }

    void print_imported() const
    {
        std::cout << "Imported keys:" << std::endl;
        size_t count = 0;
        for_each_dir_entry(logged_user_->profile().cypher_dir(), [&count](auto const &dir_entry) {
            std::cout << "* " << dir_entry.path().filename().string() << std::endl;
            ++count;
        });
        if (0 == count)
            std::cout << "There are no keys available" << std::endl;
    }

private:
    std::shared_ptr<LoggedUser> logged_user_;
};

} // namespace lmail
