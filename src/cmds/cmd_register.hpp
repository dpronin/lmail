#pragma once

#include <iostream>
#include <memory>

#include <boost/scope_exit.hpp>

#include "cli_states.hpp"
#include "storage.hpp"
#include "user.hpp"
#include "utility.hpp"
#include "types.hpp"

namespace lmail
{

class CmdRegister
{
public:
    explicit CmdRegister(args_t args, CliFsm &fsm, std::shared_ptr<Storage> storage)
        : args_(std::move(args)), fsm_(std::addressof(fsm)), storage_(std::move(storage))
    {
        if (!fsm_)
            throw std::invalid_argument("fsm provided cannot be empty");
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void operator()()
    try
    {
        username_t username;
        if (!args_.empty() && !args_.front().empty())
        {
            username = args_.front();
        }
        else if (!uread(username, "Enter user name: "))
        {
            return;
        }
        else if (username.empty())
        {
            std::cerr << "user name cannot be empty\n";
            return;
        }

        password_t password;
        if (!uread_hidden(password, "Enter a new user's password: ")) return;

        if (password.empty())
        {
            std::cerr << "password cannot be empty\n";
            return;
        }

        password_t password_repeated;
        if (!uread_hidden(password_repeated, "Repeat a new user's password: ")) return;

        BOOST_SCOPE_EXIT_ALL(&password, &password_repeated)
        {
            secure_memset(password_repeated.data(), 0, password_repeated.size());
            secure_memset(password.data(), 0, password.size());
        };

        if (password != password_repeated)
        {
            std::cerr << "incorrect password repeated\n";
            return;
        }

        // salting the password
        password.push_back(':');
        password += salt;

        User user{-1, std::move(username), sha256(password)};
        if (auto const user_id = (*storage_)->insert(user); - 1 == user_id)
            std::cerr << "couldn't register a new user '" << user.username << "'. "
                      << "User name is busy or password has incorrect format";
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
    args_t                   args_;
    CliFsm *                 fsm_;
    std::shared_ptr<Storage> storage_;
};

} // namespace lmail
