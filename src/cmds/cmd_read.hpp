#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "storage.hpp"
#include "types.hpp"
#include "user.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdRead
{
public:
    explicit CmdRead(args_t args, User const &user, std::shared_ptr<Storage> storage)
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

        auto const message_id = boost::lexical_cast<int>(args_.front());
        auto const items      = (*storage_)->select(columns(&Message::topic, &Message::body),
                                                    where(c(&Message::id) == message_id &&
                                                          c(&Message::dest_user_id) == user_->id));
        if (items.empty())
        {
            std::cerr << "message #" << message_id << " does not exist for the user " << user_->username << '\n';
            return;
        }

        if (items.size() != 1)
        {
            std::cerr << "FATAL: inconsistent data base\n";
            exit(EXIT_FAILURE);
        }

        auto const &m = items.front();
        std::cout << "\tTopic: " << std::get<0>(m) << "\n\n";
        std::cout << "\tMessage: " << '\n';
        std::cout << '\t' << std::get<1>(m) << '\n';
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
