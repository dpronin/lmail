#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include "boost/lexical_cast.hpp"

#include "sm/cli.hpp"

#include "cmd.hpp"
#include "cmd_args.hpp"
#include "color.hpp"
#include "logged_user.hpp"
#include "storage.hpp"
#include "types.hpp"
#include "uread.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdReadMsg final : public Cmd
{
    std::shared_ptr<LoggedUser> logged_user_;
    std::shared_ptr<Storage> storage_;

public:
    explicit CmdReadMsg(sm::Cli& fsm, CmdArgs args, std::shared_ptr<LoggedUser> logged_user, std::shared_ptr<Storage> storage)
        : Cmd(fsm, std::move(args))
        , logged_user_(std::move(logged_user))
        , storage_(std::move(storage))
    {
        if (!logged_user_)
            throw std::invalid_argument("logged user provided cannot be empty");
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void exec() override
    {
        fsm_.process_event(sm::ev::readmsg{{[this] { _exec_(); }}});
    }

private:
    void _exec_()
    try {
        using namespace sqlite_orm;

        auto msg_idx_str = args_.front().value_or(std::string{});
        if (msg_idx_str.empty() && !uread(msg_idx_str, "Enter message inbox index: "))
            return;

        if (msg_idx_str.empty()) {
            std::cerr << "message inbox index is not specified\n";
            return;
        }

        auto const msg_idx = boost::lexical_cast<msg_idx_t>(msg_idx_str);
        if (auto const msg_id = logged_user_->inbox().find(msg_idx)) {
            // clang-format off
            auto messages = (*storage_)->select(columns(&Message::id, &Message::topic, &Message::cyphered, &User::username, &Message::body),
                                                join<User>(on(c(&Message::orig_user_id) == &User::id)),
                                                where(c(&Message::id) == *msg_id));
            // clang-format on
            if (messages.empty()) {
                std::cerr << "message #" << msg_idx << " does not exist for the user " << logged_user_->name() << '\n';
                logged_user_->inbox().erase(msg_idx);
                return;
            }

            if (messages.size() != 1)
                throw std::runtime_error("inconsistent data base");

            logged_user_->inbox().sync(msg_idx, std::move(messages.front()));
            logged_user_->inbox().show(msg_idx, std::cout);
        } else {
            std::cerr << "There is no message #" << msg_idx << " in inbox. Sync it first\n";
        }
    } catch (boost::bad_lexical_cast const&) {
        std::cerr << cred("message ID expected as a positive integer number") << '\n';
    }
};

} // namespace lmail
