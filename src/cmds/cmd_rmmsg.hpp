#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "inbox.hpp"
#include "storage.hpp"
#include "types.hpp"

namespace lmail
{

class CmdRmMsg
{
public:
    explicit CmdRmMsg(args_t args, std::shared_ptr<User> user, std::shared_ptr<Storage> storage, std::shared_ptr<Inbox> inbox)
        : args_(std::move(args)), user_(std::move(user)), storage_(std::move(storage)), inbox_(std::move(inbox))
    {
        if (!user_)
            throw std::invalid_argument("user provided cannot be empty");
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
        if (!inbox_)
            throw std::invalid_argument("inbox provided cannot be empty");
    }

    void operator()()
    try
    {
        using namespace sqlite_orm;

        std::string msg_idx_str;
        if (!args_.empty() && !args_.front().empty())
        {
            msg_idx_str = args_.front();
        }
        else if (!uread(msg_idx_str, "Enter message inbox index: "))
        {
            return;
        }
        else if (msg_idx_str.empty())
        {
            std::cerr << "message inbox index is not specified\n";
            return;
        }

        auto const msg_idx = boost::lexical_cast<msg_idx_t>(msg_idx_str);
        if (auto const msg_id = inbox_->erase(msg_idx))
            (*storage_)->remove_all<Message>(where(c(&Message::id) == *msg_id && c(&Message::dest_user_id) == user_->id));

        std::cout << "message #" << msg_idx << " successfully removed\n";
    }
    catch (boost::bad_lexical_cast const &)
    {
        std::cerr << "error: message ID expected as a positive integer number\n";
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
    std::shared_ptr<Inbox>   inbox_;
};

} // namespace lmail
