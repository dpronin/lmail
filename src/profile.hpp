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
    std::filesystem::path profile_path_;

public:
    explicit Profile(std::filesystem::path profile_path)
        : profile_path_(std::move(profile_path))
    {
        if (profile_path_.empty())
            throw std::invalid_argument("profile path provided cannot be empty");
    }

    [[nodiscard]] std::filesystem::path const& profile_path() const noexcept { return profile_path_; }
    [[nodiscard]] std::filesystem::path keys_dir() const { return profile_path() / Application::kKeysDirName; }
    [[nodiscard]] std::filesystem::path assocs_dir() const { return profile_path() / Application::kAssocsDirName; }
    [[nodiscard]] std::filesystem::path cypher_dir() const { return profile_path() / Application::kCypherDirName; }

    [[nodiscard]] std::filesystem::path find_key(std::string_view keyname) const { return ::lmail::find_key(keys_dir(), keyname); }
    [[nodiscard]] std::filesystem::path find_assoc_key(std::string_view keyname) const { return ::lmail::find_key(assocs_dir(), keyname); }
    [[nodiscard]] std::filesystem::path find_cypher_key(std::string_view keyname) const { return ::lmail::find_key(cypher_dir(), keyname); }
};

} // namespace lmail
