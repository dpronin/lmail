#include "logged_in_state.hpp"

#include <iostream>
#include <stdexcept>

#include "color.hpp"
#include "inbox.hpp"
#include "types.hpp"

#include "cmds/cmd_inbox.hpp"
#include "cmds/cmd_logout.hpp"
#include "cmds/cmd_quit.hpp"
#include "cmds/cmd_read.hpp"
#include "cmds/cmd_remove.hpp"
#include "cmds/cmd_send.hpp"
#include "cmds/cmd_users_list.hpp"
#include "cmds/cmd_keygen.hpp"
#include "cmds/cmd_keys_list.hpp"
#include "cmds/cmd_rmkey.hpp"

using namespace lmail;

LoggedInState::LoggedInState(CliFsm &fsm, std::shared_ptr<Storage> storage, std::unique_ptr<User> user, std::shared_ptr<Inbox> inbox)
    : MainState(fsm, std::move(storage), help_cmds()), user_(std::move(user)), inbox_(std::move(inbox))
{
    if (!user_)
        throw std::invalid_argument("user provided cannot be empty");
    if (user_->username.empty())
        throw std::invalid_argument("user provided cannot be with empty name");
    if (!inbox_)
        throw std::invalid_argument("inbox provided cannot be empty");
}

help_cmds_t const &LoggedInState::help_cmds()
{
    // clang-format off
    static help_cmds_t cmds = {
        { "logout",      {},               "Logs out current user" },
        { "lsusers",     {},               "Shows all registered users" },
        { "inbox",       {},               "Shows inbox" },
        { "send",        { "[username]" }, "Initiates procedure of sending message to another user with a username specified or entered" },
        { "read",        { "[id]" },       "Reads the message with ID specified or entered" },
        { "remove",      { "[id]" },       "Removes the message with ID specified or entered. Removing performed from inbox and remote storage" },
        { "quit",        {},               "Quits the application" },
        { "help",        {},               "Shows this help page" },
        { "keygen",      { "[keyname]" },  "Generates and stores a new RSA key with a name specified or entered" },
        { "lskeys",      {},               "Shows all the keys generated and available for use" },
        { "rmkey",       { "[id]" },       "Remove the key with ID specified or entered" }
    };
    // clang-format on
    return cmds;
}

prompt_t LoggedInState::prompt() const { return default_colored("lmail (") + login_name() + default_colored(") > "); }

username_t LoggedInState::login_name() const { return colored(user_->username, color_e::green); }

void LoggedInState::OnEnter()
{
    CliState::OnEnter();
    std::cout << color_escape(color_e::brown) << "You're logged in as " << login_name() << color_escape_reset();
    std::cout << "\nInbox:\n";
    CmdInbox(user_, storage_, inbox_)();
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

    if ("help" == cmd)
    {
        help();
        return;
    }

    cmd_f_t cmd_f;
    if ("logout" == cmd)
        cmd_f = CmdLogout(*fsm_, storage_);
    else if ("lsusers" == cmd)
        cmd_f = CmdUsersList(user_, storage_);
    else if ("inbox" == cmd)
        cmd_f = CmdInbox(user_, storage_, inbox_);
    else if ("send" == cmd)
        cmd_f = CmdSend(std::move(args), user_, storage_);
    else if ("read" == cmd)
        cmd_f = CmdRead(std::move(args), user_, storage_, inbox_);
    else if ("remove" == cmd)
        cmd_f = CmdRemove(std::move(args), user_, storage_, inbox_);
    else if ("keygen" == cmd)
        cmd_f = CmdKeygen(std::move(args), Application::instance().lmail_path() / user_->username);
    else if ("rmkey" == cmd)
        cmd_f = CmdRmkey(std::move(args), Application::instance().lmail_path() / user_->username);
    else if ("lskeys" == cmd)
        cmd_f = CmdKeysList(Application::instance().lmail_path() / user_->username);
    else if ("quit" == cmd)
        cmd_f = CmdQuit(*fsm_);

    if (cmd_f)
        cmd_f();
    else
        std::cerr << "unknown command '" << cmd << "'\n";
}
