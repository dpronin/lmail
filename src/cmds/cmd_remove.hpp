#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "storage.hpp"
#include "types.hpp"

namespace lmail
{

class CmdRemove
{
public:
    explicit CmdRemove(args_t args, User const &user, std::shared_ptr<Storage> storage)
        : args_(std::move(args)), user_(std::addressof(user)), storage_(std::move(storage))
    {
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void operator()()
    try
    {
        using namespace sqlite_orm;
        if (args_.empty())
        {
            std::cerr << "message ID expected\n";
            return;
        }
        auto const message_id = boost::lexical_cast<msg_id_t>(args_.front());
        (*storage_)->remove_all<Message>(where(c(&Message::id) == message_id && c(&Message::dest_user_id) == user_->id));
        std::cout << "message #" << message_id << " successfully removed\n";
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
