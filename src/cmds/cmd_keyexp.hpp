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

class CmdKeyExp final : public Cmd
{
    std::shared_ptr<LoggedUser> logged_user_;

public:
    explicit CmdKeyExp(sm::Cli& fsm, CmdArgs args, std::shared_ptr<LoggedUser> logged_user)
        : Cmd(fsm, std::move(args))
        , logged_user_(std::move(logged_user))
    {
        if (!logged_user_)
            throw std::invalid_argument("logged user provided cannot be empty");
    }

    void exec() override
    {
        fsm_.process_event(sm::ev::keyexp{{[this] { _exec_(); }}});
    }

private:
    void _exec_()
    {
        namespace fs = std::filesystem;

        auto keyname = keyname_t{args_.front().value_or(keyname_t{})};
        if (keyname.empty() && !uread(keyname, "Enter key name: "))
            return;

        if (keyname.empty()) {
            std::cerr << "key name is not specified\n";
            return;
        }

        auto const& keys_pair_dir = logged_user_->profile().find_key(keyname);
        if (keys_pair_dir.empty()) {
            std::cerr << "There is no key '" << keyname << "'\n";
            return;
        }

        std::string copy_to_str;
        fs::path key_path_dst = Application::instance().key_path(keyname);
        key_path_dst += Application::kPubKeySuffix;
        if (!uread(copy_to_str, "Where is key '" + keyname + "' to be exported to? (default: " + key_path_dst.string() + "): "))
            return;
        if (!copy_to_str.empty())
            key_path_dst = fs::path(std::move(copy_to_str));
        std::cout << "exporting key '" << keyname << "' to " << key_path_dst << " ..." << std::endl;

        auto key_path_src = keys_pair_dir / Application::kPrivKeyName;
        key_path_src += Application::kPubKeySuffix;
        std::error_code ec;
        if (fs::copy_file(key_path_src, key_path_dst, fs::copy_options::overwrite_existing, ec))
            std::cout << "successfully exported key '" << keyname << " to " << key_path_dst << "'\n";
        else
            std::cerr << "failed to export key '" << keyname << "', reason: " << ec.message() << '\n';
    }
};

} // namespace lmail
