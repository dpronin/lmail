#pragma once

#include <memory>
#include <string_view>

#include <sqlite_orm/sqlite_orm.h>

#include "message.hpp"
#include "user.hpp"

namespace lmail
{

inline decltype(auto) make_sqlite3_storage(std::string_view db_path)
{
    using namespace sqlite_orm;
    return make_storage(db_path.data(), make_table("users",
                                                   make_column("id",       &User::id,       autoincrement(), primary_key()),
                                                   make_column("username", &User::username, unique()),
                                                   make_column("password", &User::password)),
                                        make_table("messages",
                                                   make_column("id",           &Message::id,           autoincrement(), primary_key()),
                                                   make_column("orig_user_id", &Message::orig_user_id),
                                                   make_column("dest_user_id", &Message::dest_user_id),
                                                   make_column("topic",        &Message::topic),
                                                   make_column("body",         &Message::body),
                                                   foreign_key(&Message::orig_user_id).references(&User::id),
                                                   foreign_key(&Message::dest_user_id).references(&User::id)));
}

class Storage
{
public:
    explicit Storage(std::string_view db_path) : storage_(make_sqlite3_storage(db_path))
    {
    }
    auto *      operator-> () noexcept { return std::addressof(storage_); }
    auto const *operator-> () const noexcept { return std::addressof(storage_); }

private:
    decltype(make_sqlite3_storage(std::string_view{})) storage_;
};

} // namespace lmail
