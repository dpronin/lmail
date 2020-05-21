#pragma once

#include <string>

#include "user.hpp"

namespace lmail
{

using msg_id_t = int;
using topic_t  = std::string;
using body_t   = std::string;

struct Message
{
    msg_id_t  id;
    user_id_t orig_user_id;
    user_id_t dest_user_id;
    topic_t   topic;
    body_t    body;
};

} // namespace lmail
