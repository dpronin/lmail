#pragma once

#include <system_error>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "types.hpp"

namespace lmail
{

class CmdRmkey
{
public:
    explicit CmdRmkey(args_t args, std::filesystem::path profile_path)
        : args_(std::move(args))
        , profile_path_(std::move(profile_path))
    {
        if (profile_path_.empty())
            throw std::invalid_argument("profile path provided cannot be empty");
    }

    void operator()()
    try
    {
        std::string key_idx_str;
        if (!args_.empty() && !args_.front().empty())
        {
            key_idx_str = args_.front();
        }
        else if (!uread(key_idx_str, "Enter key index: "))
        {
            return;
        }
        else if (key_idx_str.empty())
        {
            std::cerr << "key index is not specified\n";
            return;
        }

        auto const key_id = boost::lexical_cast<size_t>(key_idx_str);
        if (0 == key_id)
        {
            std::cerr << "key ID cannot be 0\n";
            return;
        }

        std::error_code ec;
        auto rng = std::filesystem::directory_iterator(profile_path_ / kKeysDir, ec);
        auto key_it = std::filesystem::begin(rng);
        std::optional<std::string> key;
        for (auto key_id_tmp = key_id; key_it != std::filesystem::end(rng); ++key_it)
        {
            if (key_it->is_directory())
            {
                if (auto key_name = key_it->path().filename().string();
                    !key_name.empty() && '.' == key_name.front())
                {
                    if (--key_id_tmp == 0)
                    {
                        key = {key_name.cbegin() + 1, key_name.cend()};
                        break;
                    }
                }
            }
        }

        if (key)
        {
            std::cout << "The key '" << *key << "' is about to be removed" << std::endl;
            std::string ans;
            while (uread(ans, "Remove it? (y/n): ") && ans != "y" && ans != "n")
                ;
            if ("y" == ans)
            {
                std::filesystem::remove_all(key_it->path(), ec);
                if (!ec)
                    std::cout << "key #" << key_id << " successfully removed\n";
                else
                    std::cerr << "error occurred when removing key #" << key_id << '\n';
            }
        }
        else
        {
            std::cerr << "There is no key #" << key_id << '\n';
        }
    }
    catch (boost::bad_lexical_cast const &)
    {
        std::cerr << "error: key ID expected as a positive integer number\n";
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
    static constexpr char kKeysDir[] = "keys";

private:
    args_t                args_;
    std::filesystem::path profile_path_;
};

} // namespace lmail
