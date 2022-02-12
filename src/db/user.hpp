#pragma once

#include <string>

namespace lmail
{

using user_id_t  = int;
using username_t = std::string;
using password_t = std::string;

struct User {
    user_id_t id;
    username_t username;
    password_t password;
};

} // namespace lmail
