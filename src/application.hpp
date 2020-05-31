#pragma once

#include <climits>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <utility>

#include <string_view>

#include <boost/process/environment.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <cryptopp/aes.h>

#include "db/user.hpp"

#include "types.hpp"

namespace lmail
{

class Application final
{
public:
    static Application &instance() noexcept
    {
        static Application app;
        return app;
    }

    struct Conf
    {
        db_path_t db_path;
    };

public:
    static Conf parse_conf(std::string_view conf_path = {})
    {
        Conf conf;

        if (conf_path.empty())
            conf_path = kDefaultConfPath;

        try
        {
            boost::property_tree::ptree conf_tree;
            read_ini(conf_path.data(), conf_tree);
            if (auto db_path = conf_tree.get_optional<std::string>("db"))
                conf.db_path = std::move(*db_path);
        }
        catch (std::exception const &ex)
        {
            std::cout << "WARNING: couldn't read configuration file " << conf_path
                      << ". Reason: " << ex.what() << '\n'
                      << "Use internal default values" << std::endl;
        }

        if (conf.db_path.empty())
            conf.db_path = kDefaultDbPath;

        return conf;
    }

    auto const& home_path() const noexcept { return home_path_; }
    auto lmail_path() const noexcept { return home_path_ / kLmailDirName; }
    auto profile_path(User const &user) const { return lmail_path() / user.username; }

private:
    Application() : home_path_(boost::this_process::environment()["HOME"].to_string())
    {
        if (home_path_.empty())
            throw std::logic_error("to continue working with the application HOME env variable is supposed to be exported");
    }

    // application properties
public:
    static constexpr char kLmailDirName[]      = ".lmail";
    static constexpr char kDefaultConfPath[]   = CONF_PREFIX "/etc/lmail.conf";
    static constexpr char kDefaultDbPath[]     = SCHEMA_DB_PREFIX "/lmail/schema.db";
    static constexpr char kSalt[]              = "2cipo6snetwdvhf384qbnxgyar51z7";
    static constexpr char kPrivKeyName[]       = "key";
    static constexpr char kPubKeySuffix[]      = ".pub";
    static constexpr char kKeysDirName[]       = ".keys";
    static constexpr char kAssocsDirName[]     = ".assocs";
    static constexpr char kCypherDirName[]     = "cypher";
    static constexpr char kUserKeyLinkSuffix[] = ".key";
    // clang-format off
    static constexpr size_t kMinRSAKeyLen        =
        std::max(static_cast<size_t>(::CryptoPP::AES::DEFAULT_KEYLENGTH), static_cast<size_t>(::CryptoPP::AES::BLOCKSIZE)) * CHAR_BIT;
    static constexpr size_t kDefaultRSAKeySize   =
        std::max(kMinRSAKeyLen, static_cast<size_t>(3072u));
    // clang-format on

private:
    std::filesystem::path home_path_;
};

} // namespace lmail
