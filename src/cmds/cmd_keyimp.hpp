#pragma once

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

#include "sm/cli.hpp"

#include "application.hpp"
#include "cmd.hpp"
#include "cmd_args.hpp"
#include "logged_user.hpp"
#include "types.hpp"
#include "uread.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdKeyImp final : public Cmd
{
    std::shared_ptr<LoggedUser> logged_user_;

public:
    explicit CmdKeyImp(sm::Cli& fsm, CmdArgs args, std::shared_ptr<LoggedUser> logged_user)
        : Cmd(fsm, std::move(args))
        , logged_user_(std::move(logged_user))
    {
        if (!logged_user_)
            throw std::invalid_argument("logged user provided cannot be empty");
    }

    void exec() override
    {
        fsm_.process_event(sm::ev::keyimp{{[this] { _exec_(); }}});
    }

private:
    void _exec_()
    {
        namespace fs = std::filesystem;

        auto args = args_;

        auto key_path_str = args.pop().value_or(std::string{});
        if (key_path_str.empty() && !uread(key_path_str, "Enter public key path: "))
            return;

        if (key_path_str.empty()) {
            std::cerr << cred("key path is not specified") << '\n';
            return;
        }

        fs::path key_path_src{std::move(key_path_str)};
        if (!fs::is_regular_file(key_path_src)) {
            std::cerr << "key " << key_path_src << " is not a file with public key\n";
            return;
        }

        auto const& cypher_dir = logged_user_->profile().cypher_dir();
        if (!create_dir_if_doesnt_exist(cypher_dir))
            throw std::runtime_error("failed to import a public key");

        auto username_tgt = username_t{args.pop().value_or(username_t{})};
        if (username_tgt.empty() && !uread(username_tgt, "Enter a target user that the key is linked to: "))
            return;

        username_tgt = fs::path(username_tgt).filename();
        if (username_tgt.empty()) {
            std::cerr << cred("target user name cannot be empty") << '\n';
            return;
        }

        if (username_tgt == logged_user_->name()) {
            std::cerr << cred("you cannot import the key from yourself") << '\n';
            return;
        }

        auto key_path_dst = cypher_dir / username_to_keyname(username_tgt);
        key_path_dst += Application::kPubKeySuffix;
        if (fs::exists(key_path_dst)) {
            std::cerr << "key from the user '" << username_tgt << "' has already been imported\n";
            return;
        }

        std::error_code ec;
        if (fs::copy_file(key_path_src, key_path_dst, ec))
            std::cout << "successfully imported key " << key_path_src << " as '" << key_path_dst.filename().string() << "' and associated with user '"
                      << username_tgt << "' for cyphering\n";
        else
            std::cerr << "failed to import key '" << key_path_src << "', reason: " << ec.message() << '\n';
    }
};

} // namespace lmail
