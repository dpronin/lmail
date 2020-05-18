#include "cli_states.hpp"

#include <iostream>
#include <string>

#include <boost/algorithm/string/join.hpp>

#include "types.hpp"

#include "cmds/cmd_help.hpp"
#include "cmds/cmd_inbox.hpp"
#include "cmds/cmd_login.hpp"
#include "cmds/cmd_logout.hpp"
#include "cmds/cmd_quit.hpp"
#include "cmds/cmd_read.hpp"
#include "cmds/cmd_register.hpp"
#include "cmds/cmd_remove.hpp"
#include "cmds/cmd_send.hpp"
#include "cmds/cmd_users_list.hpp"

namespace lmail
{

// CliState

std::string CliState::prompt() { return "> "; }

std::vector<cmd_t> CliState::commands() { return {}; }

void CliState::OnEnter()
{
    if (auto cmds = commands(); !cmds.empty())
        CmdHelp(std::move(cmds))();
}

void CliState::process(args_t args)
{
    std::cerr << "cannot process '" << boost::algorithm::join(args, " ") << "'\n";
}

// MainState

std::string MainState::prompt() { return "lmail > "; }

std::vector<cmd_t> MainState::commands() { return {"login", "register", "quit", "help"}; }

MainState::MainState(CliFsm &fsm, std::shared_ptr<Storage> storage)
    : CliState(fsm), storage_(std::move(storage))
{
    if (!storage_)
        throw std::invalid_argument("storage provided cannot be empty");
}

void MainState::process(args_t args)
{
    if (args.empty())
        return;

    auto const cmd = std::move(args.front());
    args.pop_front();

    cmd_f_t cmd_f;
    if ("login" == cmd)
        cmd_f = CmdLogin(std::move(args), *fsm_, storage_);
    else if ("register" == cmd)
        cmd_f = CmdRegister(std::move(args), *fsm_, storage_);
    else if ("quit" == cmd)
        cmd_f = CmdQuit(*fsm_);
    else if ("help" == cmd)
        cmd_f = CmdHelp(commands());

    if (cmd_f)
        cmd_f();
    else
        std::cerr << "unknown command '" << cmd << "'\n";
}

// LoggedInState

LoggedInState::LoggedInState(CliFsm &fsm, User user, std::shared_ptr<Storage> storage)
    : CliState(fsm), user_(std::move(user)), storage_(std::move(storage))
{
    if (!storage_)
        throw std::invalid_argument("storage provided cannot be empty");
}

std::string LoggedInState::prompt() { return "lmail " + user_.username + " > "; }

std::vector<cmd_t> LoggedInState::commands()
{
    return {"send", "logout", "users-list", "inbox", "read", "remove", "quit", "help"};
}

void LoggedInState::OnEnter()
{
    std::cout << "You're logged in as " << user_.username << std::endl;
    CliState::OnEnter();
    std::cout << "Inbox:\n";
    CmdInbox(user_, storage_)();
}

void LoggedInState::OnExit()
{
    std::cout << "You're logged out, " << user_.username << std::endl;
    CliState::OnExit();
}

void LoggedInState::process(args_t args)
{
    if (args.empty())
        return;

    auto const cmd = std::move(args.front());
    args.pop_front();

    cmd_f_t cmd_f;
    if ("logout" == cmd)
        cmd_f = CmdLogout(*fsm_, storage_);
    else if ("users-list" == cmd)
        cmd_f = CmdUsersList(user_, storage_);
    else if ("inbox" == cmd)
        cmd_f = CmdInbox(user_, storage_);
    else if ("send" == cmd)
        cmd_f = CmdSend(std::move(args), user_, storage_);
    else if ("read" == cmd)
        cmd_f = CmdRead(std::move(args), user_, storage_);
    else if ("remove" == cmd)
        cmd_f = CmdRemove(std::move(args), user_, storage_);
    else if ("quit" == cmd)
        cmd_f = CmdQuit(*fsm_);
    else if ("help" == cmd)
        cmd_f = CmdHelp(commands());

    if (cmd_f)
        cmd_f();
    else
        std::cerr << "unknown command '" << cmd << "'\n";
}

// QuitState

QuitState::QuitState(CliFsm &fsm) : CliState(fsm)
{
}

void QuitState::OnEnter()
{
    CliState::OnEnter();
    std::cout << "quitting..." << std::endl;
}

} // namespace lmail
