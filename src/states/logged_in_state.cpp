#include "logged_in_state.hpp"

#include <stdexcept>

#include "color.hpp"
#include "logged_user_utility.hpp"
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

LoggedInState::LoggedInState(sm::Cli &fsm, std::shared_ptr<Storage> storage, std::shared_ptr<LoggedUser> user)
    // clang-format off
    : MainState(fsm, {
            { "logout",      {},                            "Logs out current user",                                                                              [=, this](args_t)     { CmdLogout{fsm_, user_, storage}(); }},
            { "lsusers",     {},                            "Shows all registered users",                                                                         [=, this](args_t)     { CmdListUsers{user_, storage}(); }},
            { "lskeys",      {},                            "Shows all RSA key pairs generated and available for use",                                            [=, this](args_t)     { CmdListKeys{user_}(); }},
            { "inbox",       {},                            "Shows inbox",                                                                                        [=, this](args_t)     { CmdInbox{user_, storage}(); }},
            { "sendmsg",     { "[username]" },              "Initiates procedure of sending message to another user with a username specified or entered",        [=, this](args_t args){ CmdSendMsg{std::move(args), user_, storage}(); }},
            { "readmsg",     { "[id]" },                    "Reads the message with ID specified or entered",                                                     [=, this](args_t args){ CmdReadMsg{std::move(args), user_, storage}(); }},
            { "rmmsg",       { "[id]" },                    "Removes the message with ID specified or entered. Removing performed from inbox and remote storage", [=, this](args_t args){ CmdRmMsg{std::move(args), user_, storage}(); }},
            { "keygen",      { "[keyname]" },               "Generates and stores a new RSA key with name specified or entered",                                  [=, this](args_t args){ CmdKeyGen{std::move(args), user_}(); }},
            { "keyexp",      { "[keyname]" },               "Exports the RSA public key with name specified or entered into a file given as path",                [=, this](args_t args){ CmdKeyExp{std::move(args), user_}(); }},
            { "keyimp",      { "[path]",    "[username]" }, "Initiates procedure of importing an RSA public key and assigning it to a target user",               [=, this](args_t args){ CmdKeyImp{std::move(args), user_}(); }},
            { "keyassoc",    { "[keyname]", "[username]" }, "Associates the RSA key with name specified or entered with a user entered",                          [=, this](args_t args){ CmdKeyAssoc{std::move(args), user_}(); }},
            { "rmkey",       { "[keyname]" },               "Removes the RSA key pair with name specified or entered",                                            [=, this](args_t args){ CmdRmKey{std::move(args), user_}(); }},
            { "rmkeyassoc",  { "[keyname]" },               "Removes an association between RSA key pair and a user",                                             [=, this](args_t args){ CmdRmKeyAssoc{std::move(args), user_}(); }},
            { "rmkeyimp",    { "[username]" },              "Removes an imported public RSA key assigned to a user",                                              [=, this](args_t args){ CmdRmKeyImp{std::move(args), user_}(); } }
        }
    )
    , user_(std::move(user))
// clang-format on
{
    if (!user_)
        throw std::invalid_argument("logged user provided cannot be empty");
}

prompt_t LoggedInState::prompt() const { return default_colored("lmail (") + make_user_name_cgreen(*user_) + default_colored(") > "); }
