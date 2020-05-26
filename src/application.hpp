#pragma once

#include <iostream>
#include <utility>
#include <filesystem>

#include <string_view>

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/process/environment.hpp>

#include "types.hpp"
#include "user.hpp"

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
    Conf parse_conf(std::string_view conf_path = {})
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


    auto const& lmail_path() const noexcept { return lmail_path_; }

    static std::filesystem::path profile_path(User const &user) { return Application::instance().lmail_path() / user.username; }
    static std::filesystem::path home_path() noexcept { return boost::this_process::environment()["HOME"].to_string(); }

private:
    Application() : lmail_path_(home_path())
    {
        if (lmail_path_.empty())
            throw std::logic_error("to continue working with the application HOME env variable is supposed to be exported");
        lmail_path_ /= kLmailDirName;
    }

// application properties
public:
    static constexpr size_t kDefaultKeySize      = 3072u;
    static constexpr char   kLmailDirName[]      = ".lmail";
    static constexpr char   kDefaultConfPath[]   = CONF_PREFIX "/etc/lmail.conf";
    static constexpr char   kDefaultDbPath[]     = SCHEMA_DB_PREFIX "/lmail/schema.db";
    static constexpr char   kSalt[]              = "2cipo6snetwdvhf384qbnxgyar51z7";
    static constexpr char   kPrivKeyName[]       = "key";
    static constexpr char   kPubKeySuffix[]      = ".pub";
    static constexpr char   kPubKeyName[]        = "key.pub";
    static constexpr char   kKeysDirName[]       = ".keys";
    static constexpr char   kAssocsDirName[]     = ".assocs";
    static constexpr char   kCypherDirName[]     = "cypher";
    static constexpr char   kUserKeyLinkSuffix[] = ".key";

private:
    std::filesystem::path lmail_path_;
};

} // namespace lmail
