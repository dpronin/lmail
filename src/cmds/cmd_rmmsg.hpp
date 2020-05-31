#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "cmd_args.hpp"
#include "logged_user.hpp"
#include "storage.hpp"
#include "types.hpp"
#include "uread.hpp"

namespace lmail
{

class CmdRmMsg final
{
public:
    explicit CmdRmMsg(CmdArgs args, std::shared_ptr<LoggedUser> logged_user, std::shared_ptr<Storage> storage)
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

        auto msg_idx_str = args_.front();
        if (msg_idx_str.empty() && !uread(msg_idx_str, "Enter message inbox index: "))
            return;

        if (msg_idx_str.empty())
        {
            std::cerr << "message inbox index is not specified\n";
            return;
        }

        auto const msg_idx = boost::lexical_cast<msg_idx_t>(msg_idx_str);
        if (auto const msg_id = logged_user_->inbox().erase(msg_idx))
        {
            (*storage_)->remove_all<Message>(where(c(&Message::id) == *msg_id && c(&Message::dest_user_id) == logged_user_->id()));
            std::cout << "message #" << msg_idx << " successfully removed\n";
        }
        else
        {
            std::cout << "There is no message #" << msg_idx << " in inbox" << std::endl;
        }
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
    CmdArgs                     args_;
    std::shared_ptr<LoggedUser> logged_user_;
    std::shared_ptr<Storage>    storage_;
};

} // namespace lmail
