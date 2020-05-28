#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <boost/range/algorithm_ext/erase.hpp>

#include "storage.hpp"
#include "types.hpp"
#include "user.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdSendMsg
{
public:
    explicit CmdSendMsg(args_t args, std::shared_ptr<User> user, std::shared_ptr<Storage> storage)
        : args_(std::move(args)), user_(std::move(user)), storage_(std::move(storage))
    {
        if (!user_)
            throw std::invalid_argument("user provided cannot be empty");
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
        boost::remove_erase_if(args_, [](auto const &arg){ return arg.empty(); });
    }

    void operator()()
    try
    {
        using namespace sqlite_orm;

        username_t username_tgt;
        if (!args_.empty())
        {
            username_tgt = args_.front();
        }
        else if (!uread(username_tgt, "Enter a target user: "))
        {
            return;
        }
        else if (username_tgt.empty())
        {
            std::cerr << "target user name cannot be empty\n";
            return;
        }

        if (username_tgt == user_->username)
        {
            std::cerr << "you cannot send messages to yourself\n";
            return;
        }

        auto const users_ids_to = (*storage_)->select(&User::id, where(c(&User::username) == username_tgt));
        if (users_ids_to.empty())
        {
            std::cerr << "user '" << username_tgt << "' does not exist\n";
            return;
        }

        if (users_ids_to.size() != 1)
        {
            std::cerr << "FATAL: inconsistent data base\n";
            exit(EXIT_FAILURE);
        }

        topic_t topic;
        if (!uread(topic, "Enter the topic: "))
            return;
        if (topic.empty())
        {
            std::cerr << "topic cannot be empty\n";
            return;
        }

        body_t body;
        if (!uread(body, "Enter the message: "))
            return;
        if (body.empty())
        {
            std::cerr << "message cannot be empty\n";
            return;
        }

        Message message{-1, user_->id, users_ids_to.front(), std::move(topic), std::move(body), false};
        if (auto const msg_id = (*storage_)->insert(message); - 1 != msg_id)
            std::cout << "message successfully sent to " << username_tgt << '\n';
        else
            std::cerr << "couldn't send a message to " << username_tgt << '\n';
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
    std::shared_ptr<User>    user_;
    std::shared_ptr<Storage> storage_;
};

} // namespace lmail
