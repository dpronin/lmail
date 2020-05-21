#pragma once

#include <deque>
#include <functional>
#include <string>

namespace lmail
{

using user_input_t = std::string;
using cmd_t        = std::string;
using cmd_f_t      = std::function<void()>;
using arg_t        = std::string;
using args_t       = std::deque<arg_t>;
using db_path_t    = std::string;
using prompt_t     = std::string;

} // namespace lmail
