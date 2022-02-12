#pragma once

#include <iostream>
#include <memory>

#include "boost/scope_exit.hpp"

#include "db/user.hpp"

#include "application.hpp"
#include "cmd_args.hpp"
#include "storage.hpp"
#include "types.hpp"
#include "uread.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdRegister final
{
public:
    explicit CmdRegister(CmdArgs args, std::shared_ptr<Storage> storage)
        : args_(std::move(args))
        , storage_(std::move(storage))
    {
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void operator()()
    try {
        username_t username = args_.front();
        if (username.empty() && !uread(username, "Enter user name: "))
            return;

        if (username.empty()) {
            std::cerr << "user name cannot be empty\n";
            return;
        }

        password_t password;
        if (!uread_hidden(password, "Enter a new user's password: "))
            return;

        if (password.empty()) {
            std::cerr << "password cannot be empty\n";
            return;
        }

        password_t password_repeated;
        if (!uread_hidden(password_repeated, "Repeat a new user's password: "))
            return;

        BOOST_SCOPE_EXIT_ALL(&password, &password_repeated)
        {
            secure_memset(password_repeated.data(), 0, password_repeated.size());
            secure_memset(password.data(), 0, password.size());
        };

        if (password != password_repeated) {
            std::cerr << "incorrect password repeated\n";
            return;
        }

        // salting the password
        password.push_back(':');
        password += Application::kSalt;

        User user{-1, std::move(username), sha3_256(password)};
        if (auto const user_id = (*storage_)->insert(user); - 1 != user_id)
            std::cout << "Successfully registered a user " << cgreen(user.username) << std::endl;
        else
            std::cerr << "couldn't register a new user '" << user.username << "'. "
                      << "User name is busy or password has incorrect format\n";
    } catch (std::exception const& ex) {
        std::cerr << "error occurred: " << ex.what() << '\n';
    } catch (...) {
        std::cerr << "unknown exception\n";
    }

private:
    CmdArgs args_;
    std::shared_ptr<Storage> storage_;
};

} // namespace lmail
