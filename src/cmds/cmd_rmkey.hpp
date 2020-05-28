#pragma once

#include <system_error>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <algorithm>

#include "types.hpp"
#include "utility.hpp"
#include "cmd_base_args.hpp"

namespace lmail
{

class CmdRmKey final : CmdBaseArgs
{
public:
    explicit CmdRmKey(args_t args, std::filesystem::path profile_path)
        : CmdBaseArgs(std::move(args))
        , profile_path_(std::move(profile_path))
    {
        if (profile_path_.empty())
            throw std::invalid_argument("profile path provided cannot be empty");
    }

    void operator()()
    try
    {
        namespace fs = std::filesystem;

        key_name_t keyname;
        if (!args_.empty())
        {
            keyname = args_.front();
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

        if (auto const key_pair_path = find_key(profile_path_ / Application::kKeysDirName, keyname); !key_pair_path.empty())
        {
            std::cout << "The key '" << keyname << "' is about to be removed" << std::endl;
            std::string ans;
            while (uread(ans, "Remove the key and the associations linked to it? (y/n): ") && ans != "y" && ans != "n")
                ;
            if ("y" == ans)
            {
                // remove all the associations linked to this key
                for_each_dir_entry_if(profile_path_ / Application::kAssocsDirName,
                                      [&key_pair_path](auto const &dir_entry){
                                          std::error_code ec;
                                          return fs::read_symlink(dir_entry.path(), ec) == key_pair_path && !ec;
                                      },
                                      [](auto const &dir_entry){ std::error_code ec; fs::remove(dir_entry.path(), ec); });
                std::error_code ec;
                fs::remove_all(key_pair_path, ec);
                if (!ec)
                    std::cout << "key '" << keyname << "' successfully removed\n";
                else
                    std::cerr << "failed to remove key '" << keyname << "'\n";
            }
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
    std::filesystem::path profile_path_;
};

} // namespace lmail
