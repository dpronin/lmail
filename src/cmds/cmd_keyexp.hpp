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
#include "application.hpp"
#include "cmd_base_args.hpp"

namespace lmail
{

class CmdKeyExp final : CmdBaseArgs
{
public:
    explicit CmdKeyExp(args_t args, std::filesystem::path profile_path)
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

        if (auto const keys_pair_dir = find_key(profile_path_ / Application::kKeysDirName, keyname); !keys_pair_dir.empty())
        {
            std::string copy_to_str;
            fs::path key_path_dst = Application::instance().home_path() / keyname;
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
                std::cout << "successfully exported key '" << keyname << "'\n";
            else
                std::cerr << "failed to export key '" << keyname << "', reason: " << ec.message() << '\n';
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
