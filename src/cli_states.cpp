#include "cli_states.hpp"

#include <iostream>
#include <string>
#include <iterator>

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include "types.hpp"
#include "inbox.hpp"
#include "user.hpp"
#include "color.hpp"

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

std::string CliState::prompt() { return default_colored("!> "); }

std::string CliState::default_colored(std::string const &input) { return colored(input, color_e::red); }

cmds_t CliState::commands()
{
    auto const &r = help_cmds() | boost::adaptors::transformed([](auto const &help_cmd){
        return std::get<0>(help_cmd);
    });
    return {r.begin(), r.end()};
}

help_cmds_t const& CliState::help_cmds()
{
    static help_cmds_t cmds = {};
    return cmds;
}

void CliState::process(args_t args)
{
    std::cerr << color_escape(color_e::red) << "cannot process '";
    boost::copy(args, std::ostream_iterator<arg_t>(std::cerr, " "));
    std::cerr << color_escape_reset() << "'\n";
}

// InitState

InitState::InitState(CliFsm &fsm) : CliState(fsm)
{
}

// MainState

help_cmds_t const& MainState::help_cmds()
{
    static help_cmds_t cmds = {
        { "login",    { "[username]" }, "Logs in as a user with username specified or entered"    },
        { "register", { "[username]" }, "Registers a new user with username specified or entered" },
        { "quit",     {},               "Quits the application"                                   },
        { "help",     {},               "Shows this help page"                                    }
    };
    return cmds;
}

std::string MainState::prompt() { return default_colored("lmail > "); }

std::string MainState::default_colored(std::string const &input) { return colored(input, color_e::brown); }

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
        cmd_f = CmdHelp(help_cmds());

    if (cmd_f)
        cmd_f();
    else
        std::cerr << "unknown command '" << cmd << "'\n";
}

// LoggedInState

help_cmds_t const& LoggedInState::help_cmds()
{
    static help_cmds_t cmds = {
        { "logout", {},
          "Logs out current user" },
        { "users-list", {},
          "Shows all registered users" },
        { "inbox", {},
          "Shows inbox" },
        { "send", { "[username]" },
          "Initiates procedure of sending message to another user with a username specified or entered" },
        { "read", { "[id]" },
          "Reads the message with ID specified or entered" },
        { "remove", { "[id]" },
          "Removes the message with ID specified or entered. Removing performed from inbox and remote storage" },
        { "quit", {},
          "Quits the application" },
        { "help", {},
          "Shows this help page" }
    };
    return cmds;
}

std::string LoggedInState::prompt() { return default_colored("lmail (") + login_name() + default_colored(") > "); }

std::string LoggedInState::login_name() { return colored(user_.username, color_e::green); }

LoggedInState::LoggedInState(CliFsm &fsm, std::shared_ptr<Storage> storage, User user, std::shared_ptr<Inbox> inbox)
    : MainState(fsm, std::move(storage))
    , user_(std::move(user))
    , inbox_(std::move(inbox))
{
    if (!inbox_)
        throw std::invalid_argument("inbox provided cannot be empty");
}

void LoggedInState::OnEnter()
{
    CliState::OnEnter();
    std::cout << color_escape(color_e::brown) << "You're logged in as " << login_name() << color_escape_reset();
    std::cout << "\nInbox:\n";
    CmdInbox(user_, storage_, inbox_, std::cout)();
}

void LoggedInState::OnExit()
{
    std::cout << "You're logged out, " << login_name() << std::endl;
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
        cmd_f = CmdInbox(user_, storage_, inbox_, std::cout);
    else if ("send" == cmd)
        cmd_f = CmdSend(std::move(args), user_, storage_);
    else if ("read" == cmd)
        cmd_f = CmdRead(std::move(args), user_, storage_, inbox_);
    else if ("remove" == cmd)
        cmd_f = CmdRemove(std::move(args), user_, storage_, inbox_);
    else if ("quit" == cmd)
        cmd_f = CmdQuit(*fsm_);
    else if ("help" == cmd)
        cmd_f = CmdHelp(help_cmds());

    if (cmd_f)
        cmd_f();
    else
        std::cerr << "unknown command '" << cmd << "'\n";
}

} // namespace lmail
