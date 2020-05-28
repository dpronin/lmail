#pragma once

#include <system_error>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <algorithm>

#include <boost/range/algorithm_ext/erase.hpp>

#include "types.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdRmKey
{
public:
    explicit CmdRmKey(args_t args, std::filesystem::path profile_path)
        : args_(std::move(args))
        , profile_path_(std::move(profile_path))
    {
        if (profile_path_.empty())
            throw std::invalid_argument("profile path provided cannot be empty");
        boost::remove_erase_if(args_, [](auto const &arg){ return arg.empty(); });
    }

    void operator()()
    try
    {
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

        if (auto const key_path = find_key(profile_path_ / Application::kKeysDirName, keyname); !key_path.empty())
        {
            std::cout << "The key '" << keyname << "' is about to be removed" << std::endl;
            std::string ans;
            while (uread(ans, "Remove it? (y/n): ") && ans != "y" && ans != "n")
                ;
            if ("y" == ans)
            {
                std::error_code ec;
                std::filesystem::remove_all(key_path, ec);
                if (!ec)
                    std::cout << "key '" << keyname << "' successfully removed\n";
                else
                    std::cerr << "failed to remove key '" << keyname << "'\n";
            }
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
    static constexpr char kKeysDirName[] = "keys";

private:
    args_t                args_;
    std::filesystem::path profile_path_;
};

} // namespace lmail
