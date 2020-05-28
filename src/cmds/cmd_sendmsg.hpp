#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <fstream>

#include <cryptopp/rsa.h>

#include "storage.hpp"
#include "types.hpp"
#include "user.hpp"
#include "utility.hpp"
#include "cmd_base_args.hpp"

namespace lmail
{

class CmdSendMsg final : CmdBaseArgs
{
public:
    explicit CmdSendMsg(args_t args, std::shared_ptr<User> user, std::shared_ptr<Storage> storage, std::filesystem::path profile_path)
        : CmdBaseArgs(std::move(args)), user_(std::move(user)), storage_(std::move(storage)), profile_path_(std::move(profile_path))
    {
        if (!user_)
            throw std::invalid_argument("user provided cannot be empty");
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void operator()()
    try
    {
        using namespace sqlite_orm;
        namespace fs = std::filesystem;

        username_t username_tgt;
        if (!args_.empty())
        {
            username_tgt = args_.front();
        }
        else if (!uread(username_tgt, "Enter a target user the message is sent to: "))
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

        bool cyphered = false;
        if (auto const key_path = find_key(profile_path_ / Application::kCypherDirName, username_to_keyname(username_tgt)); !key_path.empty())
        {
            std::string ans;
            while (uread(ans, "Would you like to cypher message? (y/n): ") && ans != "y" && ans != "n")
                ;
            if ("y" == ans)
            {
                encrypt(key_path, topic, body);
                cyphered = true;
            }
        }
        else
        {
            std::cout << "The message will be sent as plain text" << std::endl;
        }

        std::string ans;
        while (uread(ans, "Send the message? (y/n): ") && ans != "y" && ans != "n")
            ;
        if (ans == "n")
            return;

        Message message{-1, user_->id, users_ids_to.front(), std::move(topic), std::move(body), cyphered};
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
    std::shared_ptr<User>    user_;
    std::shared_ptr<Storage> storage_;
    std::filesystem::path    profile_path_;
};

} // namespace lmail
