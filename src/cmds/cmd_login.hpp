#pragma once

#include <cstdlib>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include "boost/scope_exit.hpp"

#include "application.hpp"
#include "cli_sm.hpp"
#include "cmd_args.hpp"
#include "logged_user.hpp"
#include "logged_user_utility.hpp"
#include "storage.hpp"
#include "types.hpp"
#include "uread.hpp"
#include "utility.hpp"

#include "states/logged_in_state.hpp"

namespace lmail
{

class CmdLogin final
{
public:
    explicit CmdLogin(CmdArgs args, sm::Cli &fsm, std::shared_ptr<Storage> storage)
        : args_(std::move(args)), fsm_(fsm), storage_(std::move(storage))
    {
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void operator()()
    try
    {
        username_t username = args_.front();
        if (username.empty() && !uread(username, "Enter user name: "))
            return;

        if (username.empty())
        {
            std::cerr << "user name cannot be empty\n";
            return;
        }

        password_t password;
        if (!uread_hidden(password, "Enter password: "))
            return;

        if (password.empty())
        {
            std::cerr << "password cannot be empty\n";
            return;
        }

        BOOST_SCOPE_EXIT_ALL(&password) { secure_memset(password.data(), 0, password.size()); };

        bool b_success;
        using namespace sqlite_orm;
        if (auto users = (*storage_)->get_all<User>(where(c(&User::username) == username)); (b_success = !users.empty()))
        {
            if (1 != users.size())
            {
                std::cerr << "FATAL: inconsistent data base\n";
                exit(EXIT_FAILURE);
            }

            auto &user = users.front();

            // salting the password
            password.push_back(':');
            password += Application::kSalt;

            b_success = user.username == username && user.password == sha3_256(password);
            if (b_success)
            {
                auto logged_user  = std::make_shared<LoggedUser>(std::move(user));
                auto user_greeter = make_logged_user_greeter(logged_user, storage_);
                auto new_state    = std::make_shared<LoggedInState>(fsm_, storage_, logged_user);
                fsm_.process_event(sm::ev::login{std::move(new_state), std::move(user_greeter)});
            }
        }

        if (!b_success)
            std::cerr << "incorrect user name or password provided, or user doesn't exist\n";
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
    CmdArgs                  args_;
    sm::Cli &                fsm_;
    std::shared_ptr<Storage> storage_;
};

} // namespace lmail
