#pragma once

#include <deque>
#include <functional>
#include <string>

namespace lmail
{

using user_input_t = std::string;
using cmd_t        = std::string;
using cmd_f_t      = std::function<void()>;
using args_t       = std::deque<std::string>;

} // namespace lmail
