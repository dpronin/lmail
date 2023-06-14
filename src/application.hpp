#pragma once

#include <climits>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "boost/process/environment.hpp"
#include "boost/property_tree/ini_parser.hpp"
#include "boost/property_tree/ptree.hpp"

#include "cryptopp/aes.h"

#include "db/user.hpp"

#include "crypt.hpp"
#include "types.hpp"

namespace lmail
{

class Application final
{
public:
    static Application& instance()
    {
        static Application app;
        return app;
    }

    struct Conf {
        db_path_t db_path;
    };

    static Conf parse_conf(std::string_view conf_path = {})
    {
        Conf conf;

        if (conf_path.empty())
            conf_path = kDefaultConfPath;

        try {
            boost::property_tree::ptree conf_tree;
            read_ini(conf_path.data(), conf_tree);
            if (auto db_path = conf_tree.get_optional<std::string>("db"))
                conf.db_path = std::move(*db_path);
        } catch (std::exception const& ex) {
            std::cout << "WARNING: couldn't read configuration file " << conf_path << ". Reason: " << ex.what() << '\n'
                      << "Use internal default values" << std::endl;
        }

        if (conf.db_path.empty())
            conf.db_path = kDefaultDbPath;

        return conf;
    }

    [[nodiscard]] auto const& home_path() const noexcept { return home_path_; }
    [[nodiscard]] auto lmail_path() const noexcept { return home_path_ / kLmailDirName; }
    [[nodiscard]] auto profile_path(User const& user) const { return lmail_path() / user.username; }

    static void store(rsa_key_pair_t const& rsa_key_pair, std::filesystem::path const& keys_pair_dir)
    {
        namespace fs = std::filesystem;
        if (!create_dir_if_doesnt_exist(keys_pair_dir))
            throw std::runtime_error("couldn't create key pair");
        auto key_path = keys_pair_dir / Application::kPrivKeyName;
        save_key(rsa_key_pair.first, key_path, S_IRUSR | S_IWUSR);
        key_path += Application::kPubKeySuffix;
        save_key(rsa_key_pair.second, key_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    }

private:
    Application()
        : home_path_(boost::this_process::environment()["HOME"].to_string())
    {
        if (home_path_.empty())
            throw std::logic_error("to continue working with the application HOME env variable is supposed to be exported");
    }

    // application properties
public:
    static constexpr std::string_view kLmailDirName      = ".lmail";
    static constexpr std::string_view kDefaultConfPath   = CONF_PREFIX "/etc/lmail.conf";
    static constexpr std::string_view kDefaultDbPath     = SCHEMA_DB_PREFIX "/lmail/schema.db";
    static constexpr std::string_view kSalt              = "2cipo6snetwdvhf384qbnxgyar51z7";
    static constexpr std::string_view kPrivKeyName       = "key";
    static constexpr std::string_view kPubKeySuffix      = ".pub";
    static constexpr std::string_view kKeysDirName       = ".keys";
    static constexpr std::string_view kAssocsDirName     = ".assocs";
    static constexpr std::string_view kCypherDirName     = "cypher";
    static constexpr std::string_view kUserKeyLinkSuffix = ".key";
    // clang-format off
    static constexpr size_t kMinRSAKeyLen        =
        std::max(static_cast<size_t>(::CryptoPP::AES::DEFAULT_KEYLENGTH), static_cast<size_t>(::CryptoPP::AES::BLOCKSIZE)) * CHAR_BIT;
    static constexpr size_t kDefaultRSAKeySize   =
        std::max(kMinRSAKeyLen, static_cast<size_t>(RSA_KEY_SIZE_DEFAULT));
    // clang-format on

private:
    std::filesystem::path home_path_;
};

} // namespace lmail
