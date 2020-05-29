#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>

#include "application.hpp"
#include "inbox.hpp"
#include "profile.hpp"
#include "user.hpp"

namespace lmail
{

class LoggedUser
{
public:
    explicit LoggedUser(User user)
        : user_(std::move(user)), profile_(std::make_shared<Profile>(Application::profile_path(user_))), inbox_(profile_)
    {
        if (user_.username.empty())
            throw std::invalid_argument("user provided cannot be with empty name");
    }

    User const &   user() const noexcept { return user_; }
    Profile const &profile() const noexcept { return *profile_; }

    Inbox &      inbox() noexcept { return inbox_; }
    Inbox const &inbox() const noexcept { return inbox_; }

private:
    User                     user_;
    std::shared_ptr<Profile> profile_;
    Inbox                    inbox_;
};

} // namespace lmail
