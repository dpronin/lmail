#pragma once

#include <filesystem>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "application.hpp"
#include "utility.hpp"

namespace lmail
{

class Profile
{
public:
    explicit Profile(std::filesystem::path profile_path)
        : profile_path_(std::move(profile_path))
    {
        if (profile_path_.empty())
            throw std::invalid_argument("profile path provided cannot be empty");
    }

    std::filesystem::path const& profile_path() const noexcept { return profile_path_; }
    std::filesystem::path keys_dir() const { return profile_path() / Application::kKeysDirName; }
    std::filesystem::path assocs_dir() const { return profile_path() / Application::kAssocsDirName; }
    std::filesystem::path cypher_dir() const { return profile_path() / Application::kCypherDirName; }

    std::filesystem::path find_key(std::string_view keyname) const { return ::lmail::find_key(keys_dir(), keyname); }
    std::filesystem::path find_assoc_key(std::string_view keyname) const { return ::lmail::find_key(assocs_dir(), keyname); }
    std::filesystem::path find_cypher_key(std::string_view keyname) const { return ::lmail::find_key(cypher_dir(), keyname); }

private:
    std::filesystem::path profile_path_;
};

} // namespace lmail
