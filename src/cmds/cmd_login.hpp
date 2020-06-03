#pragma once

#include <cstdlib>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <boost/scope_exit.hpp>

#include "application.hpp"
#include "cmd_args.hpp"
#include "logged_user.hpp"
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
    explicit CmdLogin(CmdArgs args, CliFsm &cli_fsm, std::shared_ptr<Storage> storage)
        : args_(std::move(args)), cli_fsm_(std::addressof(cli_fsm)), storage_(std::move(storage))
    {
        if (!cli_fsm_)
            throw std::invalid_argument("fsm provided cannot be empty");
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
                cli_fsm_->change_state(std::make_shared<LoggedInState>(*cli_fsm_, storage_, std::make_shared<LoggedUser>(std::move(user))));
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
    CliFsm *                 cli_fsm_;
    std::shared_ptr<Storage> storage_;
};

} // namespace lmail
