#pragma once

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "db/user.hpp"

#include "application.hpp"
#include "inbox.hpp"
#include "profile.hpp"

namespace lmail
{

class LoggedUser
{
    User user_;
    std::shared_ptr<Profile> profile_;
    Inbox inbox_;

public:
    explicit LoggedUser(User user)
        : user_(std::move(user))
        , profile_(std::make_shared<Profile>(Application::instance().profile_path(user_)))
        , inbox_(profile_)
    {
        if (name().empty())
            throw std::invalid_argument("user provided cannot be with empty name");
    }

    [[nodiscard]] user_id_t id() const noexcept { return user_.id; }
    [[nodiscard]] username_t const& name() const noexcept { return user_.username; }
    [[nodiscard]] password_t const& password() const noexcept { return user_.password; }

    [[nodiscard]] Profile const& profile() const noexcept { return *profile_; }

    Inbox& inbox() noexcept { return inbox_; }
    [[nodiscard]] Inbox const& inbox() const noexcept { return inbox_; }
};

} // namespace lmail
