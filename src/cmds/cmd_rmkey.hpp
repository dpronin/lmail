#pragma once

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

#include "cmd_args.hpp"
#include "logged_user.hpp"
#include "types.hpp"
#include "uread.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdRmKey final
{
public:
    explicit CmdRmKey(CmdArgs args, std::shared_ptr<LoggedUser> logged_user)
        : args_(std::move(args))
        , logged_user_(std::move(logged_user))
    {
        if (!logged_user_)
            throw std::invalid_argument("logged user provided cannot be empty");
    }

    void operator()()
    try {
        namespace fs = std::filesystem;

        keyname_t keyname = args_.front();
        if (keyname.empty() && !uread(keyname, "Enter key name: "))
            return;

        if (keyname.empty()) {
            std::cerr << "key name is not specified\n";
            return;
        }

        auto const& key_pair_path = logged_user_->profile().find_key(keyname);
        if (key_pair_path.empty()) {
            std::cerr << "There is no key '" << keyname << "'\n";
            return;
        }

        std::cout << "The key '" << keyname << "' is about to be removed" << std::endl;
        std::string ans;
        while (uread(ans, "Remove the key and the associations linked to it? (y/n): ") && ans != "y" && ans != "n")
            ;
        if ("y" == ans) {
            // remove all the associations linked to this key
            for_each_dir_entry_if(
                logged_user_->profile().assocs_dir(),
                [&key_pair_path](auto const& dir_entry) {
                    std::error_code ec;
                    return fs::read_symlink(dir_entry.path(), ec) == key_pair_path && !ec;
                },
                [](auto const& dir_entry) {
                    std::error_code ec;
                    fs::remove(dir_entry.path(), ec);
                });
            std::error_code ec;
            fs::remove_all(key_pair_path, ec);
            if (!ec)
                std::cout << "key '" << keyname << "' successfully removed\n";
            else
                std::cerr << "failed to remove key '" << keyname << "'\n";
        }
    } catch (std::exception const& ex) {
        std::cerr << "error occurred: " << ex.what() << '\n';
    } catch (...) {
        std::cerr << "unknown exception\n";
    }

private:
    CmdArgs args_;
    std::shared_ptr<LoggedUser> logged_user_;
};

} // namespace lmail
