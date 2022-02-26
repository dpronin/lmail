#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include "cmd_interface.hpp"
#include "logged_user.hpp"
#include "storage.hpp"

namespace lmail
{

class CmdListUsers final : public ICmd
{
    std::shared_ptr<LoggedUser> logged_user_;
    std::shared_ptr<Storage> storage_;

public:
    explicit CmdListUsers(std::shared_ptr<LoggedUser> logged_user, std::shared_ptr<Storage> storage)
        : logged_user_(std::move(logged_user))
        , storage_(std::move(storage))
    {
        if (!logged_user_)
            throw std::invalid_argument("logged user provided cannot be empty");
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void exec() override
    try {
        auto usernames = (*storage_)->select(&User::username);
        if (usernames.empty())
            std::cout << "There are no users\n";
        else if (usernames.size() == 1)
            std::cout << "There is 1 user:\n";
        else
            std::cout << "There are " << usernames.size() << " users:\n";
        for (auto const& username : usernames) {
            std::cout << "* " << username;
            if (username == logged_user_->name())
                std::cout << " (" << cgreen("me") << ")";
            std::cout << '\n';
        }
    } catch (std::exception const& ex) {
        std::cerr << "error occurred: " << ex.what() << '\n';
    } catch (...) {
        std::cerr << "unknown exception\n";
    }
};

} // namespace lmail
