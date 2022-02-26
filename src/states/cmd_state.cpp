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

#include "cmds/cmd_help.hpp"

#include "cmd_interface.hpp"
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
    std::get<3>(cmds_.back()) = [cmd = std::make_shared<CmdHelp>(std::cout, cmds_)](args_t) { return cmd; };
}
// clang-format on

cmds_names_t CmdState::cmds() const
{
    // clang-format off
    auto const range = cmds_ | boost::adaptors::transformed([](auto const &cmd) { return std::get<0>(cmd); });
    // clang-format on
    return {range.begin(), range.end()};
}

std::shared_ptr<ICmd> CmdState::parse(args_t args)
{
    std::shared_ptr<ICmd> cmd;

    if (args.empty())
        return cmd;

    auto const cmd_name = std::move(args.front());
    args.pop_front();

    if (auto it = boost::find_if(cmds_, [&](auto const& cmd) { return std::get<0>(cmd) == cmd_name; }); cmds_.end() != it) {
        cmd = std::get<3>(*it)(std::move(args));
    } else {
        std::cerr << "unknown command '" << cmd_name << "'\n";
        std::cerr << cred("cannot process") << " '" << cmd_name << ' ';
        boost::copy(args, std::ostream_iterator<arg_t>(std::cerr, " "));
        std::cerr << "\b'\n";
    }

    return cmd;
}
