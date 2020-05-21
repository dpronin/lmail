#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "storage.hpp"
#include "types.hpp"
#include "inbox.hpp"

namespace lmail
{

class CmdRemove
{
public:
    explicit CmdRemove(args_t args, User const &user, std::shared_ptr<Storage> storage, std::shared_ptr<Inbox> inbox)
        : args_(std::move(args)), user_(std::addressof(user)), storage_(std::move(storage)), inbox_(std::move(inbox))
    {
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

        auto const msg_idx  = boost::lexical_cast<msg_idx_t>(msg_idx_str);
        if (auto const msg_id = inbox_->erase(msg_idx))
            (*storage_)->remove_all<Message>(where(c(&Message::id) == *msg_id && c(&Message::dest_user_id) == user_->id));

        std::cout << "message #" << msg_idx << " successfully removed\n";
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
    std::shared_ptr<Inbox>   inbox_;
};

} // namespace lmail
