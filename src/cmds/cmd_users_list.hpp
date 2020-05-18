#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include "storage.hpp"
#include "user.hpp"

namespace lmail
{

class CmdUsersList
{
public:
    explicit CmdUsersList(User const &user, std::shared_ptr<Storage> storage)
        : user_(std::addressof(user)), storage_(std::move(storage))
    {
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void operator()()
    try
    {
        auto const usernames = (*storage_)->select(&User::username);
        if (usernames.empty())
            std::cout << "There are no users\n";
        else if (usernames.size() == 1)
            std::cout << "There is 1 user:\n";
        else
            std::cout << "There are " << usernames.size() << " users:\n";
        for (auto const &username : usernames)
        {
            std::cout << "* " << username;
            if (username == user_->username)
                std::cout << " (me)";
            std::cout << '\n';
        }
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
    User const *             user_;
    std::shared_ptr<Storage> storage_;
};

} // namespace lmail
