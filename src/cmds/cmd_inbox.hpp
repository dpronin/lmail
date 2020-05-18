#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include "message.hpp"
#include "storage.hpp"
#include "user.hpp"

namespace lmail
{

class CmdInbox
{
public:
    explicit CmdInbox(User const &user, std::shared_ptr<Storage> storage)
        : user_(std::addressof(user)), storage_(std::move(storage))
    {
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void operator()()
    try
    {
        using namespace sqlite_orm;
        auto const items = (*storage_)->select(columns(&Message::id, &Message::topic), where(c(&Message::dest_user_id) == user_->id));
        if (items.empty())
            std::cout << "There are no messages\n";
        else if (items.size() == 1)
            std::cout << "There is 1 message:\n";
        else
            std::cout << "There are " << items.size() << " messages:\n";
        for (auto const &[id, topic] : items)
            std::cout << '\t' << id << ". " << topic << '\n';
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
};

} // namespace lmail
