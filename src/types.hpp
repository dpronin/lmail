#pragma once

#include <deque>
#include <functional>
#include <string>
#include <tuple>
#include <vector>

#include "fsm.hpp"

namespace lmail
{

using user_input_t = std::string;
using cmd_t        = std::string;
using cmds_t       = std::vector<cmd_t>;
using arg_t        = std::string;
using args_t       = std::deque<arg_t>;
using desc_cmd_t   = std::string;
using help_cmd_t   = std::tuple<cmd_t, args_t, desc_cmd_t>;
using help_cmds_t  = std::vector<help_cmd_t>;
using cmd_f_t      = std::function<void()>;
using db_path_t    = std::string;
using prompt_t     = std::string;
using key_fname_t  = std::string;

class CliState;
using CliFsm = Fsm<CliState>;

} // namespace lmail
