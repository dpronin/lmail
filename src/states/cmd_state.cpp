#include "cmd_state.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include "boost/range/adaptor/transformed.hpp"
#include "boost/range/algorithm/adjacent_find.hpp"
#include "boost/range/algorithm/copy.hpp"
#include "boost/range/algorithm/find_if.hpp"
#include "boost/range/algorithm/sort.hpp"
#include "boost/range/numeric.hpp"

#include "cmds/cmd_help.hpp"

#include "color.hpp"
#include "types.hpp"

using namespace lmail;

// clang-format off
CmdState::CmdState(cmds_t cmds) : cmds_(std::move(cmds))
{
    boost::sort(cmds_, [](auto const &cmd1, auto const &cmd2){ return std::get<0>(cmd1) < std::get<0>(cmd2); });
    if (auto it = boost::adjacent_find(cmds_, [](auto const &cmd1, auto const &cmd2){ return std::get<0>(cmd1) == std::get<0>(cmd2); }); cmds_.end() != it)
        throw std::invalid_argument("commands given are not unique, cmd=" + std::get<0>(*it));
    cmds_.push_back({"help", {}, "Shows this help page", {}});
    std::get<3>(cmds_.back()) = [show_help = CmdHelp{cmds_}](args_t) { show_help(std::cout); };
}
// clang-format on

cmds_names_t CmdState::cmds() const
{
    // clang-format off
    auto const range = cmds_ | boost::adaptors::transformed([](auto const &cmd) { return std::get<0>(cmd); });
    // clang-format on
    return {range.begin(), range.end()};
}

prompt_t CmdState::prompt() const { return default_colored("!> "); }

std::string CmdState::default_colored(std::string_view input) const { return cred(input); }

void CmdState::process(args_t args)
{
    if (args.empty())
        return;

    auto const cmd_name = std::move(args.front());
    args.pop_front();

    // clang-format off
    if (auto it = boost::find_if(cmds_, [&](auto const &cmd) { return std::get<0>(cmd) == cmd_name; }); cmds_.end() != it)
    // clang-format on
    {
        std::get<3> (*it)(std::move(args));
    }
    else
    {
        std::cerr << "unknown command '" << cmd_name << "'\n";
        std::cerr << default_colored("cannot process") << " '" << cmd_name << ' ';
        boost::copy(args, std::ostream_iterator<arg_t>(std::cerr, " "));
        std::cerr << "\b'\n";
    }
}
