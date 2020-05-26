#pragma once

#include <cstdlib>

#include <system_error>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <algorithm>

#include "types.hpp"
#include "utility.hpp"
#include "application.hpp"

namespace lmail
{

class CmdRmKeyAssoc
{
public:
    explicit CmdRmKeyAssoc(args_t args, std::filesystem::path profile_path)
        : args_(std::move(args))
        , profile_path_(std::move(profile_path))
    {
        if (profile_path_.empty())
            throw std::invalid_argument("profile path provided cannot be empty");
    }

    void operator()()
    try
    {
        namespace fs = std::filesystem;

        username_t username_tgt;
        if (!args_.empty() && !args_.front().empty())
        {
            username_tgt = args_.front();
        }
        else if (!uread(username_tgt, "Enter a user name: "))
        {
            return;
        }
        else if (username_tgt.empty())
        {
            std::cerr << "user name is not specified\n";
            return;
        }

        auto extract_keyname = [](auto const &assoc_path){
            std::error_code ec;
            return fs::read_symlink(assoc_path, ec).filename().string();
        };

        if (auto const assoc_path = find_assoc(profile_path_ / Application::kAssocsDirName, username_tgt); !assoc_path.empty())
        {
            auto const keyname = extract_keyname(assoc_path);
            if (keyname.empty())
            {
                std::cerr << "couldn't extract key name\n";
                return;
            }

            std::cout << "The association between key '" << keyname << "' and user '" << username_tgt << "' is about to be removed" << std::endl;
            std::string ans;
            while (uread(ans, "Remove it? (y/n): ") && ans != "y" && ans != "n")
                ;
            if ("y" == ans)
            {
                std::error_code ec;
                fs::remove(assoc_path, ec);
                if (!ec)
                    std::cout << "association successfully removed\n";
                else
                    std::cerr << "failed to remove the association\n";
            }
        }
        else
        {
            std::cerr << "There is none key associated with user '" << username_tgt << "'\n";
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
