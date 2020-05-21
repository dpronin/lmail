#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "storage.hpp"
#include "types.hpp"
#include "user.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdRead
{
public:
    explicit CmdRead(args_t args, User const &user, std::shared_ptr<Storage> storage)
        : args_(std::move(args)), user_(std::addressof(user)), storage_(std::move(storage))
    {
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void operator()()
    try
    {
        using namespace sqlite_orm;

        std::string msg_id;
        if (!args_.empty() && !args_.front().empty())
        {
            msg_id = args_.front();
        }
        else if (!uread(msg_id, "Enter message ID: "))
        {
            return;
        }
        else if (msg_id.empty())
        {
            std::cerr << "message ID is not specified\n";
            return;
        }

        auto const message_id = boost::lexical_cast<int>(msg_id);
        auto const messages   = (*storage_)->select(columns(&Message::topic, &Message::body),
                                                    where(c(&Message::id) == message_id &&
                                                          c(&Message::dest_user_id) == user_->id));
        if (messages.empty())
        {
            std::cerr << "message #" << message_id << " does not exist for the user " << user_->username << '\n';
            return;
        }

        if (messages.size() != 1)
        {
            std::cerr << "FATAL: inconsistent data base\n";
            exit(EXIT_FAILURE);
        }

        auto const &msg = messages.front();
        std::cout << "\tTopic: " << std::get<0>(msg) << "\n\n";
        std::cout << "\tMessage: " << '\n';
        std::cout << '\t' << std::get<1>(msg) << '\n';
    }
    catch (boost::bad_lexical_cast const &)
    {
        std::cerr << "error: message ID expected as an integer number\n";
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
    User const *             user_;
    std::shared_ptr<Storage> storage_;
};

} // namespace lmail
