#include "cmd_state.hpp"

#include <iostream>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <utility>

#include "cmds/cmd_help.hpp"

#include "cmd_interface.hpp"
#include "color.hpp"
#include "types.hpp"

using namespace lmail;

// clang-format off
CmdState::CmdState(cmds_t cmds) : cmds_(std::move(cmds))
{
    std::ranges::sort(cmds_, std::less<>{}, [](auto const &cmd){ return std::get<0>(cmd); });
    if (auto it = std::ranges::adjacent_find(cmds_, [](auto const &cmd1, auto const &cmd2){ return std::get<0>(cmd1) == std::get<0>(cmd2); }); cmds_.end() != it)
        throw std::invalid_argument("commands given are not unique, cmd=" + std::get<0>(*it));
    cmds_.push_back({"help", {}, "Shows this help page", {}});
    std::get<3>(cmds_.back()) = [this](args_t) { return std::make_unique<CmdHelp>(std::cout, cmds_); };
}
// clang-format on

cmds_names_t CmdState::cmds() const
{
    auto const range = cmds_ | std::views::transform([](auto const& cmd) { return std::get<0>(cmd); });
    return {range.begin(), range.end()};
}

std::unique_ptr<ICmd> CmdState::parse(args_t args)
{
    std::unique_ptr<ICmd> cmd;

    if (args.empty())
        return cmd;

    auto const cmd_name = std::move(args.front());
    args.pop_front();

    if (auto it = std::ranges::find_if(cmds_, [&](auto const& cmd) { return std::get<0>(cmd) == cmd_name; }); cmds_.end() != it) {
        cmd = std::get<3>(*it)(std::move(args));
    } else {
        std::cerr << "unknown command '" << cmd_name << "'\n";
        std::cerr << cred("cannot process") << " '" << cmd_name << ' ';
        std::ranges::copy(args, std::ostream_iterator<arg_t>(std::cerr, " "));
        std::cerr << "\b'\n";
    }

    return cmd;
}
