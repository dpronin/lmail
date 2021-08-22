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
#include "cmds/cmd_rmkeyimp.hpp"
#include "cmds/cmd_rmmsg.hpp"
#include "cmds/cmd_sendmsg.hpp"

using namespace lmail;

LoggedInState::LoggedInState(sm::Cli &fsm, std::shared_ptr<Storage> storage, std::shared_ptr<LoggedUser> logged_user)
    // clang-format off
    : MainState(fsm, std::move(storage), {
            { "logout",      {},                            "Logs out current user",                                                                              [=](args_t){ CmdLogout{fsm_, storage_}(); }},
            { "lsusers",     {},                            "Shows all registered users",                                                                         [=](args_t){ CmdListUsers{logged_user_, storage_}(); }},
            { "lskeys",      {},                            "Shows all RSA key pairs generated and available for use",                                            [=](args_t){ CmdListKeys{logged_user_}(); }},
            { "inbox",       {},                            "Shows inbox",                                                                                        [=](args_t){ CmdInbox{logged_user_, storage_}(); }},
            { "sendmsg",     { "[username]" },              "Initiates procedure of sending message to another user with a username specified or entered",        [=](args_t args){ CmdSendMsg{std::move(args), logged_user_, storage_}(); }},
            { "readmsg",     { "[id]" },                    "Reads the message with ID specified or entered",                                                     [=](args_t args){ CmdReadMsg{std::move(args), logged_user_, storage_}(); }},
            { "rmmsg",       { "[id]" },                    "Removes the message with ID specified or entered. Removing performed from inbox and remote storage", [=](args_t args){ CmdRmMsg{std::move(args), logged_user_, storage_}(); }},
            { "keygen",      { "[keyname]" },               "Generates and stores a new RSA key with name specified or entered",                                  [=](args_t args){ CmdKeyGen{std::move(args), logged_user_}(); }},
            { "keyexp",      { "[keyname]" },               "Exports the RSA public key with name specified or entered into a file given as path",                [=](args_t args){ CmdKeyExp{std::move(args), logged_user_}(); }},
            { "keyimp",      { "[path]",    "[username]" }, "Initiates procedure of importing an RSA public key and assigning it to a target user",               [=](args_t args){ CmdKeyImp{std::move(args), logged_user_}(); }},
            { "keyassoc",    { "[keyname]", "[username]" }, "Associates the RSA key with name specified or entered with a user entered",                          [=](args_t args){ CmdKeyAssoc{std::move(args), logged_user_}(); }},
            { "rmkey",       { "[keyname]" },               "Removes the RSA key pair with name specified or entered",                                            [=](args_t args){ CmdRmKey{std::move(args), logged_user_}(); }},
            { "rmkeyassoc",  { "[keyname]" },               "Removes an association between RSA key pair and a user",                                             [=](args_t args){ CmdRmKeyAssoc{std::move(args), logged_user_}(); }},
            { "rmkeyimp",    { "[username]" },              "Removes an imported public RSA key assigned to a user",                                              [=](args_t args){ CmdRmKeyImp{std::move(args), logged_user_}(); } }
        }
    )
    , logged_user_(std::move(logged_user))
// clang-format on
{
    if (!logged_user_)
        throw std::invalid_argument("logged user provided cannot be empty");
}

prompt_t LoggedInState::prompt() const { return default_colored("lmail (") + login_name() + default_colored(") > "); }

username_t LoggedInState::login_name() const { return cgreen(logged_user_->name()); }

void LoggedInState::OnEnter()
{
    CliState::OnEnter();
    std::cout << color_escape(color_e::brown) << "You're logged in as " << login_name() << color_escape_reset();
    std::cout << "\nInbox:\n";
    CmdInbox(logged_user_, storage_)();
}

void LoggedInState::OnExit()
{
    std::cout << "You're logged out, " << login_name() << std::endl;
    CliState::OnExit();
}
