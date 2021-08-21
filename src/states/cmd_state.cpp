#include "cmd_state.hpp"

#include <iostream>
#include <iterator>
#include <memory>

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/transform.hpp>

#include "color.hpp"

using namespace lmail;

CmdState::CmdState(help_cmds_t help_cmds) : help_(std::move(help_cmds))
{
    cmds_.reserve(help_.help_cmds().size());
    // clang-format off
    boost::transform(help_.help_cmds(), std::back_inserter(cmds_), [](auto const &help_cmd){
        return std::get<0>(help_cmd);
    });
    // clang-format on
}

prompt_t CmdState::prompt() const { return default_colored("!> "); }

std::string CmdState::default_colored(std::string_view input) const { return cred(input); }

void CmdState::process(args_t args)
{
    std::cerr << default_colored("cannot process") << " '";
    boost::copy(args, std::ostream_iterator<arg_t>(std::cerr, " "));
    std::cerr << "'\n";
}

void CmdState::help() { help_(); }
