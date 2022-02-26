#include "cmd_logout.hpp"

#include <memory>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "logged_user_utility.hpp"
#include "storage.hpp"
#include "types.hpp"

#include "sm/cli.hpp"

#include "cmd_help.hpp"
#include "cmd_interface.hpp"
#include "cmd_login.hpp"
#include "cmd_parser_default.hpp"
#include "cmd_quit.hpp"
#include "cmd_register.hpp"

#include "states/cmd_state.hpp"

using namespace lmail;

CmdLogout::CmdLogout(sm::Cli& fsm, std::shared_ptr<LoggedUser> user, std::shared_ptr<Storage> storage)
    : Cmd(fsm)
    , user_(std::move(user))
    , storage_(std::move(storage))
{
    if (!user_)
        throw std::invalid_argument("user provided cannot be empty");
    if (!storage_)
        throw std::invalid_argument("storage provided cannot be empty");
}

void CmdLogout::exec()
{
    auto cmds             = cmds_with_help({
        // clang-format off
        {"login",    {"[username]"}, "Logs in as a user with username specified or entered",    [&f = fsm_, s = storage_, this](args_t args) { return std::make_unique<CmdLogin>(f, std::move(args), s); }},
        {"register", {"[username]"}, "Registers a new user with username specified or entered", [&f = fsm_, s = storage_      ](args_t args) { return std::make_unique<CmdRegister>(f, std::move(args), s); }},
        {"quit",     {            }, "Quits the application",                                   [&f = fsm_,               this](args_t&&)    { return std::make_unique<CmdQuit>(f); }},
        // clang-format on
    });
    auto const cmds_names = cmds | std::views::transform([](auto const& cmd) { return std::get<0>(cmd); });
    fsm_.process_event(sm::ev::logout{
        std::make_shared<CmdState>(cmds_names_t{cmds_names.begin(), cmds_names.end()}, colorize::to_yellow("lmail > ")),
        std::make_shared<CmdParserDefault>(std::move(cmds)),
        make_logged_user_exiter(user_)});
}
