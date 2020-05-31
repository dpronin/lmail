#pragma once

#include <vector>

#include "user.hpp"

namespace lmail
{

using msg_id_t     = int;
using topic_blob_t = std::vector<char>;
using body_blob_t  = std::vector<char>;

struct Message
{
    msg_id_t     id;
    user_id_t    orig_user_id;
    user_id_t    dest_user_id;
    topic_blob_t topic;
    body_blob_t  body;
    bool         cyphered;
};

} // namespace lmail
