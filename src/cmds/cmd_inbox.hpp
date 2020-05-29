#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include "logged_user.hpp"
#include "message.hpp"
#include "storage.hpp"

namespace lmail
{

class CmdInbox final
{
public:
    explicit CmdInbox(std::shared_ptr<LoggedUser> logged_user, std::shared_ptr<Storage> storage)
        : logged_user_(std::move(logged_user)), storage_(std::move(storage))
    {
        if (!logged_user_)
            throw std::invalid_argument("logged user provided cannot be empty");
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void operator()()
    try
    {
        using namespace sqlite_orm;
        auto items                              = (*storage_)->select(columns(&Message::id, &Message::topic, &Message::cyphered, &User::username), join<User>(on(c(&Message::orig_user_id) == &User::id)), where(c(&Message::dest_user_id) == logged_user_->user().id));
        auto const [old_messages, new_messages] = logged_user_->inbox().sync(std::move(items));
        auto const all_messages                 = old_messages + new_messages;
        if (0 != all_messages)
        {
            if (1 == all_messages)
                std::cout << "There is 1 " << (1 == new_messages ? "new" : "") << " message:\n";
            else
                std::cout << "There are " << all_messages << " messages (" << new_messages << " new):\n";
            logged_user_->inbox().show_topics(std::cout);
        }
        else
        {
            std::cout << "There are no messages\n";
        }
        std::cout.flush();
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
    std::shared_ptr<LoggedUser> logged_user_;
    std::shared_ptr<Storage>    storage_;
};

} // namespace lmail
