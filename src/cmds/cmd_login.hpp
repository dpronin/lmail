#pragma once

#include <cstdlib>

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "db/user.hpp"

#include "sm/cli.hpp"

#include "application.hpp"
#include "color.hpp"
#include "crypt.hpp"
#include "logged_user.hpp"
#include "logged_user_utility.hpp"
#include "storage.hpp"
#include "types.hpp"
#include "uread.hpp"
#include "utility.hpp"

#include "cmd.hpp"
#include "cmd_args.hpp"
#include "cmd_help.hpp"
#include "cmd_inbox.hpp"
#include "cmd_keyassoc.hpp"
#include "cmd_keyexp.hpp"
#include "cmd_keygen.hpp"
#include "cmd_keyimp.hpp"
#include "cmd_list_keys.hpp"
#include "cmd_list_users.hpp"
#include "cmd_logout.hpp"
#include "cmd_parser_default.hpp"
#include "cmd_quit.hpp"
#include "cmd_readmsg.hpp"
#include "cmd_rmkey.hpp"
#include "cmd_rmkeyassoc.hpp"
#include "cmd_rmkeyimp.hpp"
#include "cmd_rmmsg.hpp"
#include "cmd_sendmsg.hpp"

#include "states/cmd_state.hpp"

namespace lmail
{

class CmdLogin final : public Cmd
{
    std::shared_ptr<Storage> storage_;

public:
    explicit CmdLogin(sm::Cli& fsm, CmdArgs args, std::shared_ptr<Storage> storage)
        : Cmd(fsm, std::move(args))
        , storage_(std::move(storage))
    {
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void exec() override
    {
        fsm_.process_event(sm::ev::login{{[this] { _exec_(); }}});
    }

private:
    void _exec_()
    {
        auto username = username_t{args_.front().value_or(username_t{})};
        if (username.empty() && !uread(username, "Enter user name: "))
            return;

        if (username.empty()) {
            std::cerr << cred("user name cannot be empty") << '\n';
            return;
        }

        auto password = make_secure<password_t>();
        if (!uread_hidden(*password, "Enter password: "))
            return;

        if (password->empty()) {
            std::cerr << cred("password cannot be empty") << '\n';
            return;
        }

        bool b_success{false};
        using namespace sqlite_orm;
        if (auto users = (*storage_)->get_all<User>(where(c(&User::username) == username)); (b_success = !users.empty())) {
            if (1 != users.size())
                throw std::runtime_error("inconsistent data base");

            auto& user = users.front();

            // salting the password
            password->push_back(':');
            (*password) += Application::kSalt;

            b_success = user.username == username && user.password == sha3_256(*password);
            if (b_success) {
                auto logged_user      = std::make_shared<LoggedUser>(std::move(user));
                auto user_greeter     = make_logged_user_greeter(fsm_, logged_user, storage_);
                auto cmds             = cmds_with_help({
                    // clang-format off
                    { "logout",      {                           }, "Logs out current user",                                                                              [&f = fsm_, u = logged_user, s = storage_, this](args_t)     { return std::make_unique<CmdLogout>(f, u, s); }},
                    { "lsusers",     {                           }, "Shows all registered users",                                                                         [&f = fsm_, u = logged_user, s = storage_, this](args_t)     { return std::make_unique<CmdListUsers>(f, u, s); }},
                    { "lskeys",      {                           }, "Shows all RSA key pairs generated and available for use",                                            [&f = fsm_, u = logged_user,               this](args_t)     { return std::make_unique<CmdListKeys>(f, u); }},
                    { "inbox",       {                           }, "Shows inbox",                                                                                        [&f = fsm_, u = logged_user, s = storage_, this](args_t)     { return std::make_unique<CmdInbox>(f, u, s); }},
                    { "sendmsg",     { "[username]"              }, "Initiates procedure of sending message to another user with a username specified or entered",        [&f = fsm_, u = logged_user, s = storage_, this](args_t args){ return std::make_unique<CmdSendMsg>(f, std::move(args), u, s); }},
                    { "readmsg",     { "[id]"                    }, "Reads the message with ID specified or entered",                                                     [&f = fsm_, u = logged_user, s = storage_, this](args_t args){ return std::make_unique<CmdReadMsg>(f, std::move(args), u, s); }},
                    { "rmmsg",       { "[id]"                    }, "Removes the message with ID specified or entered. Removing performed from inbox and remote storage", [&f = fsm_, u = logged_user, s = storage_, this](args_t args){ return std::make_unique<CmdRmMsg>(f, std::move(args), u, s); }},
                    { "keygen",      { "[keyname]"               }, "Generates and stores a new RSA key with name specified or entered",                                  [&f = fsm_, u = logged_user,               this](args_t args){ return std::make_unique<CmdKeyGen>(f, std::move(args), u); }},
                    { "keyexp",      { "[keyname]"               }, "Exports the RSA public key with name specified or entered into a file given as path",                [&f = fsm_, u = logged_user,               this](args_t args){ return std::make_unique<CmdKeyExp>(f, std::move(args), u); }},
                    { "keyimp",      { "[path]",    "[username]" }, "Initiates procedure of importing an RSA public key and assigning it to a target user",               [&f = fsm_, u = logged_user,               this](args_t args){ return std::make_unique<CmdKeyImp>(f, std::move(args), u); }},
                    { "keyassoc",    { "[keyname]", "[username]" }, "Associates the RSA key with name specified or entered with a user entered",                          [&f = fsm_, u = logged_user,               this](args_t args){ return std::make_unique<CmdKeyAssoc>(f, std::move(args), u); }},
                    { "rmkey",       { "[keyname]"               }, "Removes the RSA key pair with name specified or entered",                                            [&f = fsm_, u = logged_user,               this](args_t args){ return std::make_unique<CmdRmKey>(f, std::move(args), u); }},
                    { "rmkeyassoc",  { "[keyname]"               }, "Removes an association between RSA key pair and a user",                                             [&f = fsm_, u = logged_user,               this](args_t args){ return std::make_unique<CmdRmKeyAssoc>(f, std::move(args), u); }},
                    { "rmkeyimp",    { "[username]"              }, "Removes an imported public RSA key assigned to a user",                                              [&f = fsm_, u = logged_user,               this](args_t args){ return std::make_unique<CmdRmKeyImp>(f, std::move(args), u); }},
                    { "quit",        {                           }, "Quits the application",                                                                              [&f = fsm_,                                this](args_t&&)   { return std::make_unique<CmdQuit>(f); }},
                    // clang-format on
                });
                auto const cmds_names = cmds | std::views::transform([](auto const& cmd) { return std::get<0>(cmd); });
                auto oss{std::ostringstream{}};
                oss << colorize::cyellow("lmail (") << make_user_name_cgreen(*logged_user) << colorize::cyellow(") > ");
                auto new_state = std::make_shared<CmdState>(cmds_names_t{cmds_names.begin(), cmds_names.end()}, oss.str());
                fsm_.process_event(sm::ev::login_finish{
                    {
                        std::move(new_state),
                        std::make_shared<CmdParserDefault>(std::move(cmds)),
                    },
                    std::move(user_greeter),
                });
            }
        }

        if (!b_success)
            std::cerr << cred("incorrect user name or password provided, or user doesn't exist") << '\n';
    }
};

} // namespace lmail
