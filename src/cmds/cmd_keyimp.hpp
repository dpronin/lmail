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

class CmdKeyImp
{
public:
    explicit CmdKeyImp(std::filesystem::path profile_path) : profile_path_(std::move(profile_path))
    {
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
        if (fs::exists(cypher_dir) && !fs::is_directory(cypher_dir) || !fs::exists(cypher_dir) && !fs::create_directory(cypher_dir))
        {
            std::cerr << "couldn't create " << cypher_dir << '\n';
            return;
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
