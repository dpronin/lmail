#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include "message.hpp"
#include "storage.hpp"
#include "user.hpp"
#include "inbox.hpp"

namespace lmail
{

class CmdInbox
{
public:
    explicit CmdInbox(User const &user, std::shared_ptr<Storage> storage, std::shared_ptr<Inbox> inbox)
        : user_(std::addressof(user)), storage_(std::move(storage)), inbox_(std::move(inbox))
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
        auto items = (*storage_)->select(columns(&Message::id, &Message::topic), where(c(&Message::dest_user_id) == user_->id));
        inbox_->sync(std::move(items));
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
    User const *             user_;
    std::shared_ptr<Storage> storage_;
    std::shared_ptr<Inbox>   inbox_;
};

} // namespace lmail
