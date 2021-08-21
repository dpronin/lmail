#pragma once

#include <cstdlib>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

#include "application.hpp"
#include "cmd_args.hpp"
#include "logged_user.hpp"
#include "types.hpp"
#include "uread.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdRmKeyImp final
{
public:
    explicit CmdRmKeyImp(CmdArgs args, std::shared_ptr<LoggedUser> logged_user)
        : args_(std::move(args)), logged_user_(std::move(logged_user))
    {
        if (!logged_user_)
            throw std::invalid_argument("logged user provided cannot be empty");
    }

    void operator()()
    try
    {
        namespace fs = std::filesystem;

        username_t username_tgt = args_.front();
        if (username_tgt.empty() && !uread(username_tgt, "Enter a target user name the key is linked to: "))
            return;

        if (username_tgt.empty())
        {
            std::cerr << "target user name is not specified\n";
            return;
        }

        auto const &key_path = logged_user_->profile().find_cypher_key(username_to_keyname(username_tgt));
        if (key_path.empty())
        {
            std::cerr << "There is none public key associated with user '" << username_tgt << "'\n";
            return;
        }

        // clang-format off
        auto extract_keyname = [](auto const &key_path) { return key_path.filename().string(); };
        // clang-format on
        auto const keyname = extract_keyname(key_path);
        if (keyname.empty())
        {
            std::cerr << "couldn't extract key name\n";
            return;
        }

        std::cout << "The imported key '" << keyname << "' for user '" << username_tgt << "' is about to be removed" << std::endl;
        std::string ans;
        while (uread(ans, "Remove it? (y/n): ") && ans != "y" && ans != "n")
            ;
        if ("y" == ans)
        {
            std::error_code ec;
            fs::remove(key_path, ec);
            if (!ec)
                std::cout << "imported key '" << keyname << "' for user '" << username_tgt << "' successfully removed\n";
            else
                std::cerr << "failed to remove the imported key '" << keyname << "' for user '" << username_tgt << "'\n";
        }
    }
    catch (std::exception const &ex)
    {
        std::cerr << "error occurred: " << ex.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "unknown exception\n";
    }

private:
    CmdArgs                     args_;
    std::shared_ptr<LoggedUser> logged_user_;
};

} // namespace lmail
