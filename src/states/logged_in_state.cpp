#include "logged_in_state.hpp"

#include <iostream>
#include <stdexcept>

#include "color.hpp"
#include "inbox.hpp"
#include "types.hpp"

#include "cmds/cmd_inbox.hpp"
#include "cmds/cmd_keyassoc.hpp"
#include "cmds/cmd_keyexp.hpp"
#include "cmds/cmd_keygen.hpp"
#include "cmds/cmd_keyimp.hpp"
#include "cmds/cmd_list_keys.hpp"
#include "cmds/cmd_list_users.hpp"
#include "cmds/cmd_logout.hpp"
#include "cmds/cmd_quit.hpp"
#include "cmds/cmd_readmsg.hpp"
#include "cmds/cmd_rmkey.hpp"
#include "cmds/cmd_rmkeyassoc.hpp"
#include "cmds/cmd_rmmsg.hpp"
#include "cmds/cmd_sendmsg.hpp"
#include "cmds/cmd_rmkeyimp.hpp"

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
        { "sendmsg",     { "[username]" }, "Initiates procedure of sending message to another user with a username specified or entered" },
        { "readmsg",     { "[id]" },       "Reads the message with ID specified or entered" },
        { "rmmsg",       { "[id]" },       "Removes the message with ID specified or entered. Removing performed from inbox and remote storage" },
        { "quit",        {},               "Quits the application" },
        { "help",        {},               "Shows this help page" },
        { "keygen",      { "[keyname]" },  "Generates and stores a new RSA key with name specified or entered" },
        { "keyexp",      { "[keyname]" },  "Exports the RSA public key with name specified or entered into a file given as path" },
        { "keyimp",      {},               "Initiates procedure of importing an RSA public key and assigning it to a target user" },
        { "keyassoc",    { "[keyname]" },  "Associates the RSA key with name specified or entered with a user entered" },
        { "lskeys",      {},               "Shows all RSA key pairs generated and available for use" },
        { "rmkey",       { "[keyname]" },  "Removes the RSA key pair with name specified or entered" },
        { "rmkeyassoc",  { "[keyname]" },  "Removes an association between RSA key pair and a user" },
        { "rmkeyimp",    { "[username]" }, "Removes an imported public RSA key assigned to a user" }
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
        cmd_f = CmdListUsers(user_, storage_);
    else if ("inbox" == cmd)
        cmd_f = CmdInbox(user_, storage_, inbox_);
    else if ("sendmsg" == cmd)
        cmd_f = CmdSendMsg(std::move(args), user_, storage_, Application::profile_path(*user_));
    else if ("readmsg" == cmd)
        cmd_f = CmdReadMsg(std::move(args), user_, storage_, inbox_);
    else if ("rmmsg" == cmd)
        cmd_f = CmdRmMsg(std::move(args), user_, storage_, inbox_);
    else if ("keygen" == cmd)
        cmd_f = CmdKeyGen(std::move(args), Application::profile_path(*user_));
    else if ("rmkey" == cmd)
        cmd_f = CmdRmKey(std::move(args), Application::profile_path(*user_));
    else if ("keyexp" == cmd)
        cmd_f = CmdKeyExp(std::move(args), Application::profile_path(*user_));
    else if ("keyimp" == cmd)
        cmd_f = CmdKeyImp(user_, Application::profile_path(*user_));
    else if ("keyassoc" == cmd)
        cmd_f = CmdKeyAssoc(std::move(args), user_, Application::profile_path(*user_));
    else if ("rmkeyassoc" == cmd)
        cmd_f = CmdRmKeyAssoc(std::move(args), Application::profile_path(*user_));
    else if ("rmkeyimp" == cmd)
        cmd_f = CmdRmKeyImp(std::move(args), Application::profile_path(*user_));
    else if ("lskeys" == cmd)
        cmd_f = CmdListKeys(Application::profile_path(*user_));
    else if ("quit" == cmd)
        cmd_f = CmdQuit(*fsm_);

    if (cmd_f)
        cmd_f();
    else
        std::cerr << "unknown command '" << cmd << "'\n";
}
