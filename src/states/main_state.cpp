#include "main_state.hpp"

#include <iostream>
#include <utility>
#include <memory>

#include "color.hpp"
#include "types.hpp"

#include "cmds/cmd_login.hpp"
#include "cmds/cmd_quit.hpp"
#include "cmds/cmd_register.hpp"

using namespace lmail;

MainState::MainState(CliFsm &fsm, std::shared_ptr<Storage> storage) : MainState(fsm, std::move(storage), help_cmds())
{
}

MainState::MainState(CliFsm &fsm, std::shared_ptr<Storage> storage, help_cmds_t help_cmds)
    : CmdState(std::move(help_cmds)), fsm_(std::addressof(fsm)), storage_(std::move(storage))
{
    if (!storage_)
        throw std::invalid_argument("storage provided cannot be empty");
}

help_cmds_t const &MainState::help_cmds()
{
    // clang-format off
    static help_cmds_t cmds = {
        {"login",    {"[username]"}, "Logs in as a user with username specified or entered"},
        {"register", {"[username]"}, "Registers a new user with username specified or entered"},
        {"quit",     {},             "Quits the application"},
        {"help",     {},             "Shows this help page"}
    };
    // clang-format on
    return cmds;
}

prompt_t MainState::prompt() const { return default_colored("lmail > "); }

std::string MainState::default_colored(std::string_view input) const { return colored(input, color_e::brown); }

void MainState::process(args_t args)
{
    if (args.empty())
        return;

    auto const cmd = std::move(args.front());
    args.pop_front();

    if ("help" == cmd)
    {
        help();
        return;
    }

    cmd_f_t cmd_f;
    if ("login" == cmd)
        cmd_f = CmdLogin(std::move(args), *fsm_, storage_);
    else if ("register" == cmd)
        cmd_f = CmdRegister(std::move(args), storage_);
    else if ("quit" == cmd)
        cmd_f = CmdQuit(*fsm_);

    if (cmd_f)
        cmd_f();
    else
        std::cerr << "unknown command '" << cmd << "'\n";
}
