#pragma once

#include <iostream>
#include <memory>
#include <sstream>

#include "color.hpp"
#include "logged_user.hpp"
#include "storage.hpp"

#include "cmds/cmd_inbox.hpp"

namespace lmail
{

inline std::string make_user_name_cgreen(LoggedUser const& user)
{
    auto oss{std::ostringstream{}};
    oss << cgreen(user.name());
    return oss.str();
}

inline auto make_logged_user_greeter(std::shared_ptr<LoggedUser> user, std::shared_ptr<Storage> storage)
{
    return [=] {
        std::cout << cyellow("You're logged in as") << " " << make_user_name_cgreen(*user);
        std::cout << "\nInbox:\n";
        CmdInbox(user, storage).exec();
    };
}

inline auto make_logged_user_exiter(std::shared_ptr<LoggedUser> user)
{
    return [=] { std::cout << "You're logged out, " << make_user_name_cgreen(*user) << std::endl; };
}

} // namespace lmail
