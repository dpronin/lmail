#pragma once

#include <iostream>
#include <utility>

#include <string_view>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

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
    Conf parse_conf(std::string_view conf_path = {})
    {
        Conf conf;

        if (conf_path.empty())
            conf_path = conf_path_def;

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
            conf.db_path = db_path_def;

        return conf;
    }

private:
    Application() = default;

private:
    static constexpr char conf_path_def[] = CONF_PREFIX      "/etc/lmail.conf";
    static constexpr char db_path_def[]   = SCHEMA_DB_PREFIX "/lmail/schema.db";
};

} // namespace lmail
