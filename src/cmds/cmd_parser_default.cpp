#include "cmd_parser_default.hpp"

#include <iostream>
#include <memory>
#include <ostream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "cmds/cmd_help.hpp"

#include "color.hpp"
#include "types.hpp"
#include "utility.hpp"

using namespace lmail;

CmdParserDefault::CmdParserDefault(cmds_t cmds)
    : cmds_(std::move(cmds))
{
}

std::unique_ptr<ICmd> CmdParserDefault::parse(args_t args)
{
    std::unique_ptr<ICmd> cmd;

    args = normalize(std::move(args));
    if (args.empty())
        return cmd;

    auto const cmd_name = std::move(args.front());
    args.pop_front();

    if (auto it = std::ranges::find_if(cmds_, [&](auto const& cmd) { return std::get<0>(cmd) == cmd_name; }); cmds_.end() != it) {
        cmd = std::get<3>(*it)(std::move(args));
    } else {
        std::ostringstream oss;
        oss << "unknown command '" << cmd_name;
        if (!args.empty()) {
            std::ranges::copy(args, std::ostream_iterator<arg_t>(oss << ' ', " "));
            oss << '\b';
        }
        oss << '\'';
        std::cerr << cred(oss.str()) << '\n';
    }

    return cmd;
}
