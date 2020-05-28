#pragma once

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "application.hpp"
#include "types.hpp"
#include "utility.hpp"
#include "user.hpp"
#include "cmd_base_args.hpp"

namespace lmail
{

class CmdKeyGen final : CmdBaseArgs
{
public:
    explicit CmdKeyGen(args_t args, std::filesystem::path const &profile_path)
        : CmdBaseArgs(std::move(args))
        , keys_dir_(profile_path / Application::kKeysDirName)
    {
        if (profile_path.empty())
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
            std::cerr << "key name cannot be empty\n";
            return;
        }

        keyname = fs::path(keyname).filename();
        if (keyname.empty())
        {
            std::cerr << "key name provided cannot be empty\n";
            return;
        }

        std::cout << "Trying to add key '" << keyname << "' key...\n";

        auto keys_pair_dir = keys_dir_ / keyname;
        keys_pair_dir += Application::kUserKeyLinkSuffix;
        if (fs::exists(keys_pair_dir))
        {
            std::cerr << "key with name '" << keyname << "' already exists\n"
                      << "Select another name for a new key\n";
            return;
        }

        size_t key_size = Application::kDefaultKeySize;
        std::string key_size_str;
        if (!uread(key_size_str, "Enter a new RSA key's size (default: " + std::to_string(key_size) + "): "))
            return;
        if (!key_size_str.empty())
        {
            if (auto const key_size_tmp = boost::lexical_cast<size_t>(key_size_str); 0 != key_size_tmp)
                key_size = key_size_tmp;
            else
                std::cerr << "key size cannot be 0. Used default size " << key_size << '\n';
        }
        else
        {
            std::cerr << "key size is unspecified. Used default size " << key_size << '\n';
        }
        std::cout << "generation key '" << keyname << "', key size " << key_size << ". Wait a while ...";
        std::cout.flush();
        generate_rsa_key_pair(keys_pair_dir, key_size);
        std::cout << "\nsuccessfully generated key '" << keyname << "', key size " << key_size << std::endl;
    }
    catch (boost::bad_lexical_cast const &)
    {
        std::cerr << "error: key size expected as a positive integer number\n";
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
    std::filesystem::path keys_dir_;
};

} // namespace lmail
