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

namespace lmail
{

class CmdKeygen
{
public:
    explicit CmdKeygen(args_t args, std::filesystem::path const &profile_path)
        : args_(std::move(args))
        , keys_dir_(profile_path / kKeysDir)
    {
        if (profile_path.empty())
            throw std::invalid_argument("profile path provided cannot be empty");
    }

    void operator()()
    try
    {
        key_fname_t key_fname;
        if (!args_.empty() && !args_.front().empty())
        {
            key_fname = args_.front();
        }
        else if (!uread(key_fname, "Enter key file name: "))
        {
            return;
        }
        else if (key_fname.empty())
        {
            std::cerr << "key file name cannot be empty\n";
            return;
        }

        key_fname = std::filesystem::path(key_fname).filename().string();
        if (key_fname.empty())
        {
            std::cerr << "key name provided cannot be empty\n";
            return;
        }
        std::cout << "Trying to add key with name " << key_fname << " key...\n";

        auto const keys_dir = keys_dir_ / ("." + key_fname);
        if (std::filesystem::exists(keys_dir))
        {
            std::cerr << "key with name '" << key_fname << "' already exists\n"
                      << "Select another name for a new key\n";
            return;
        }

        size_t keysize = Application::instance().default_keysize();
        std::string keysize_str;
        if (!uread(keysize_str, "Enter a new RSA key's size (default: " + std::to_string(keysize) + "): "))
            return;
        if (!keysize_str.empty())
        {
            if (auto const keysize_tmp = boost::lexical_cast<size_t>(keysize_str); 0 != keysize_tmp)
                keysize = keysize_tmp;
            else
                std::cerr << "key size cannot be 0. Used default size " << keysize << '\n';
        }
        else
        {
            std::cerr << "key size is unspecified. Used default size " << keysize << '\n';
        }
        std::cout << "generation key '" << key_fname << "', key size " << keysize << ". Wait a while ...";
        std::cout.flush();
        create_rsa_key(keys_dir, keysize);
        std::cout << "\nsuccessfully generated key '" << key_fname << "', key size " << keysize << std::endl;
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
    static constexpr char kKeysDir[] = "keys";

private:
    args_t                args_;
    std::filesystem::path keys_dir_;
};

} // namespace lmail
