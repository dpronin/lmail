#pragma once

#include <cstddef>

#include <string>

#include "db/message.hpp"

namespace lmail
{

using msg_idx_t = size_t;
using topic_t   = std::string;
using body_t    = std::string;

struct InboxMessage {
    msg_id_t id;
    topic_t topic;
    body_t body;
    bool cyphered;
    username_t user_from;
};

} // namespace lmail
