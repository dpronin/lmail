#pragma once

#include <cstdlib>

#include <system_error>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <algorithm>

#include <boost/range/algorithm_ext/erase.hpp>

#include "types.hpp"
#include "utility.hpp"
#include "application.hpp"
#include "user.hpp"

namespace lmail
{

class CmdKeyAssoc
{
public:
    explicit CmdKeyAssoc(args_t args, std::shared_ptr<User> user, std::filesystem::path profile_path)
        : args_(std::move(args))
        , user_(std::move(user))
        , profile_path_(std::move(profile_path))
    {
        if (!user_)
            throw std::invalid_argument("user provided cannot be empty");
        if (profile_path_.empty())
            throw std::invalid_argument("profile path provided cannot be empty");
        boost::remove_erase_if(args_, [](auto const &arg){ return arg.empty(); });
    }

    void operator()()
    try
    {
        namespace fs = std::filesystem;

        auto args = args_;

        key_name_t keyname;
        if (!args.empty())
        {
            keyname = std::move(args.front());
            args.pop_front();
        }
        else if (!uread(keyname, "Enter key name: "))
        {
            return;
        }
        else if (keyname.empty())
        {
            std::cerr << "key name is not specified\n";
            return;
        }

        if (auto const keys_pair_dir = find_key_pair_dir(profile_path_ / Application::kKeysDirName, keyname); !keys_pair_dir.empty())
        {
            username_t username_tgt;
            if (!args.empty())
            {
                username_tgt = args.front();
                args.pop_front();
            }
            else if (!uread(username_tgt, "Enter a target user name: "))
            {
                return;
            }
            else if (username_tgt.empty())
            {
                std::cerr << "target user name cannot be empty\n";
                return;
            }

            username_tgt = fs::path(username_tgt).filename();
            if (username_tgt.empty())
            {
                std::cerr << "target user name cannot be empty\n";
                return;
            }

            if (user_->username == username_tgt)
            {
                std::cerr << "you cannot create a new association between your key and yourself\n";
                return;
            }

            std::cout << "trying to link key '" << keyname << "' to user '" << username_tgt << "' ...\n";

            auto const assocs_dir = profile_path_ / Application::kAssocsDirName;
            if (fs::exists(assocs_dir) && !fs::is_directory(assocs_dir) || !fs::exists(assocs_dir) && !fs::create_directory(assocs_dir))
            {
                std::cerr << "couldn't create " << assocs_dir << '\n';
                return;
            }

            auto link_path = assocs_dir / username_tgt;
            link_path += Application::kUserKeyLinkSuffix;
            if (fs::exists(link_path))
            {
                std::cerr << "some key is already associated with user '" << username_tgt << "'\n";
                return;
            }

            std::error_code ec;
            fs::create_directory_symlink(keys_pair_dir, link_path, ec);
            if (!ec)
                std::cout << "successfully created key-user association" << std::endl;
            else
                std::cerr << "failed to create association between key '" << keyname << "' and the user '"
                          << username_tgt << "', reason: " << ec.message() << '\n';
        }
        else
        {
            std::cerr << "There is no key '" << keyname << "'\n";
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
    args_t                args_;
    std::shared_ptr<User> user_;
    std::filesystem::path profile_path_;
};

} // namespace lmail
