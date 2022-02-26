#pragma once

#include <cstdlib>

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
#include "logged_user.hpp"
#include "types.hpp"
#include "uread.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdRmKeyAssoc final : public Cmd
{
    std::shared_ptr<LoggedUser> logged_user_;

public:
    explicit CmdRmKeyAssoc(sm::Cli& fsm, CmdArgs args, std::shared_ptr<LoggedUser> logged_user)
        : Cmd(fsm, std::move(args))
        , logged_user_(std::move(logged_user))
    {
        if (!logged_user_)
            throw std::invalid_argument("logged user provided cannot be empty");
    }

    void exec() override
    {
        fsm_.process_event(sm::ev::rmkeyassoc{{[this] { _exec_(); }}});
    }

private:
    void _exec_()
    {
        namespace fs = std::filesystem;

        auto username_tgt = username_t{args_.front().value_or(username_t{})};
        if (username_tgt.empty() && !uread(username_tgt, "Enter a target user name the association is linked to: "))
            return;

        if (username_tgt.empty()) {
            std::cerr << "target user name is not specified\n";
            return;
        }

        auto const& assoc_path = logged_user_->profile().find_assoc_key(username_to_keyname(username_tgt));
        if (assoc_path.empty()) {
            std::cerr << "There is none key associated with user '" << username_tgt << "'\n";
            return;
        }

        auto extract_keyname = [](auto const& assoc_path) {
            std::error_code ec;
            return fs::read_symlink(assoc_path, ec).filename().string();
        };

        auto const keyname = extract_keyname(assoc_path);
        if (keyname.empty()) {
            std::cerr << "couldn't extract key name\n";
            return;
        }

        std::cout << "The association between key '" << keyname << "' and user '" << username_tgt << "' is about to be removed" << std::endl;
        std::string ans;
        while (uread(ans, "Remove it? (y/n): ") && ans != "y" && ans != "n")
            ;
        if ("y" == ans) {
            std::error_code ec;
            fs::remove(assoc_path, ec);
            if (!ec)
                std::cout << "association between key '" << keyname << "' and user '" << username_tgt << "' successfully removed\n";
            else
                std::cerr << "failed to remove the association between key '" << keyname << "' and user '" << username_tgt << "'\n";
        }
    }
};

} // namespace lmail
