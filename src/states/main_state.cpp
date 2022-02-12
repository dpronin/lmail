#include "main_state.hpp"

#include <iostream>
#include <memory>
#include <utility>

#include "color.hpp"
#include "types.hpp"

#include "cmds/cmd_login.hpp"
#include "cmds/cmd_quit.hpp"
#include "cmds/cmd_register.hpp"

using namespace lmail;

namespace
{


} // anonymous namespace

// clang-format off
MainState::MainState(sm::Cli &fsm, std::shared_ptr<Storage> storage)
    : MainState(fsm, {
        { "login",    { "[username]" }, "Logs in as a user with username specified or entered",    [=, this](args_t args){ CmdLogin{std::move(args), fsm_, storage}(); } },
        { "register", { "[username]" }, "Registers a new user with username specified or entered", [=](args_t args){ CmdRegister{std::move(args), storage}(); } }
    })
// clang-format on
{
}

// clang-format off
MainState::MainState(sm::Cli &fsm, cmds_t cmds)
    : CmdState((cmds.push_back({"quit", {}, "Quits the application", [=](args_t){ CmdQuit{fsm_}(); }}), std::move(cmds)))
    , fsm_(fsm)
// clang-format on
{
}

prompt_t MainState::prompt() const { return default_colored("lmail > "); }

std::string MainState::default_colored(std::string_view input) const { return cbrown(input); }
