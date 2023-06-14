#pragma once

#include <memory>
#include <ranges>
#include <utility>

#include "sm/cli.hpp"

#include "cmd_help.hpp"
#include "cmd_interface.hpp"
#include "cmd_login.hpp"
#include "cmd_parser_default.hpp"
#include "cmd_quit.hpp"
#include "cmd_register.hpp"

#include "color.hpp"
#include "storage.hpp"
#include "types.hpp"

#include "states/cmd_state.hpp"

namespace lmail
{

class CmdInvoker final
{
    sm::Cli fsm_;

public:
    explicit CmdInvoker(sm::CliCtx& ctx, std::shared_ptr<Storage> storage)
        : fsm_{ctx}
    {
        auto cmds             = cmds_with_help({
            // clang-format off
            {"login",    {"[username]"}, "Logs in as a user with username specified or entered",    [=, this] (args_t args) { return std::make_unique<CmdLogin>(fsm_, std::move(args), storage); }},
            {"register", {"[username]"}, "Registers a new user with username specified or entered", [=, this] (args_t args) { return std::make_unique<CmdRegister>(fsm_, std::move(args), storage); }},
            {"quit",     {            }, "Quits the application",                                   [this]    (args_t&&)    { return std::make_unique<CmdQuit>(fsm_); }},
            // clang-format on
        });
        auto const cmds_names = cmds | std::views::transform([](auto const& cmd) { return std::get<0>(cmd); });
        fsm_.process_event(sm::ev::run{
            {
                std::make_shared<CmdState>(cmds_names_t{cmds_names.begin(), cmds_names.end()}, colorize::to_yellow("lmail > ")),
                std::make_shared<CmdParserDefault>(std::move(cmds)),
            },
        });
    }

    void operator()(std::shared_ptr<ICmd> cmd)
    {
        if (cmd)
            cmd->exec();
    }

    explicit operator bool() const noexcept
    {
        using namespace boost::sml;
        return !fsm_.is(X);
    }
};

} // namespace lmail
