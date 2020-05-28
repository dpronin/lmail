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

namespace lmail
{

class CmdKeyImp final
{
public:
    explicit CmdKeyImp(std::shared_ptr<User> user, std::filesystem::path profile_path)
        : user_(std::move(user))
        , profile_path_(std::move(profile_path))
    {
        if (!user_)
            throw std::invalid_argument("user provided cannot be empty");
        if (profile_path_.empty())
            throw std::invalid_argument("profile path provided cannot be empty");
    }

    void operator()()
    try
    {
        namespace fs = std::filesystem;

        std::string key_path_str;
        if (!uread(key_path_str, "Enter public key path: "))
        {
            return;
        }
        else if (key_path_str.empty())
        {
            std::cerr << "key path is not specified\n";
            return;
        }

        fs::path key_path_src{std::move(key_path_str)};
        if (!fs::is_regular_file(key_path_src))
        {
            std::cerr << "key " << key_path_src << " is not a file with public key\n";
            return;
        }

        auto const cypher_dir = profile_path_ / Application::kCypherDirName;
        if (fs::exists(cypher_dir) && !fs::is_directory(cypher_dir) || !fs::exists(cypher_dir) && !fs::create_directories(cypher_dir))
        {
            std::cerr << "couldn't create " << cypher_dir << '\n';
            return;
        }

        username_t username_tgt;
        if (!uread(username_tgt, "Enter a target user that the key is linked to: "))
            return;

        username_tgt = fs::path(username_tgt).filename();
        if (username_tgt.empty())
        {
            std::cerr << "target user name cannot be empty\n";
            return;
        }

        if (username_tgt == user_->username)
        {
            std::cerr << "you cannot import the key from yourself\n";
            return;
        }

        auto key_path_dst = cypher_dir / username_tgt;
        key_path_dst += Application::kPubKeySuffix;
        if (fs::exists(key_path_dst))
        {
            std::cerr << "key from the user '" << username_tgt << "' has already been imported\n";
            return;
        }

        std::error_code ec;
        if (fs::copy_file(key_path_src, key_path_dst, ec))
            std::cout << "successfully imported key " << key_path_src
                      <<  " as '" << key_path_dst.filename().string()
                      << "' and associated with user '" << username_tgt << "' for cyphering\n";
        else
            std::cerr << "failed to import key '" << key_path_src << "', reason: " << ec.message() << '\n';
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
    std::shared_ptr<User> user_;
    std::filesystem::path profile_path_;
};

} // namespace lmail
