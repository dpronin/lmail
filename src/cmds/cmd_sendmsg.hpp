#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include "boost/scope_exit.hpp"

#include "cryptopp/rsa.h"

#include "cmd_args.hpp"
#include "logged_user.hpp"
#include "storage.hpp"
#include "types.hpp"
#include "uread.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdSendMsg final
{
public:
    explicit CmdSendMsg(CmdArgs args, std::shared_ptr<LoggedUser> logged_user, std::shared_ptr<Storage> storage)
        : args_(std::move(args)), logged_user_(std::move(logged_user)), storage_(std::move(storage))
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
        namespace fs = std::filesystem;

        username_t username_tgt = args_.front();
        if (username_tgt.empty() && !uread(username_tgt, "Enter a target user the message is sent to: "))
            return;

        if (username_tgt.empty())
        {
            std::cerr << "target user name cannot be empty\n";
            return;
        }

        if (username_tgt == logged_user_->name())
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

        bool    cyphered = false;
        topic_t topic;
        body_t  body;

        BOOST_SCOPE_EXIT_ALL(&)
        {
            if (cyphered)
            {
                secure_memset(topic.data(), 0, topic.size());
                secure_memset(body.data(), 0, body.size());
            }
        };

        if (!uread(topic, "Enter the topic: "))
            return;
        if (topic.empty())
        {
            std::cerr << "topic cannot be empty\n";
            return;
        }

        if (!uread(body, "Enter the message: "))
            return;
        if (body.empty())
        {
            std::cerr << "message cannot be empty\n";
            return;
        }

        if (auto const &key_path = logged_user_->profile().find_cypher_key(username_to_keyname(username_tgt)); !key_path.empty())
        {
            std::string ans;
            while (uread(ans, "Would you like to cypher the message? (y/n): ") && ans != "y" && ans != "n")
                ;
            if ("y" == ans)
            {
                encrypt(key_path, topic, body);
                cyphered = true;
            }
        }

        if (!cyphered)
            std::cout << "The message will be sent as plain text" << std::endl;

        std::string ans;
        while (uread(ans, "Send the message? (y/n): ") && ans != "y" && ans != "n")
            ;
        if (ans == "n")
            return;

        Message message{-1, logged_user_->id(), users_ids_to.front(), {topic.cbegin(), topic.cend()}, {body.cbegin(), body.cend()}, cyphered};
        if (auto const msg_id = (*storage_)->insert(message); - 1 != msg_id)
            std::cout << "message successfully sent to " << username_tgt << '\n';
        else
            std::cerr << "couldn't send the message to " << username_tgt << '\n';
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
    void encrypt(std::filesystem::path const &key_path, topic_t &topic, body_t &body)
    try
    {
        auto const key = load_key<CryptoPP::RSA::PublicKey>(key_path);
        ::lmail::encrypt(topic, key);
        ::lmail::encrypt(body, key);
    }
    catch (std::exception const &ex)
    {
        std::cerr << "error occurred while cyphering message, reason: " << ex.what() << '\n';
    }

private:
    CmdArgs                     args_;
    std::shared_ptr<LoggedUser> logged_user_;
    std::shared_ptr<Storage>    storage_;
};

} // namespace lmail
