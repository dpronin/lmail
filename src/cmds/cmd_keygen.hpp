#pragma once

#include <cstddef>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "boost/lexical_cast.hpp"

#include "application.hpp"
#include "cmd_args.hpp"
#include "cmd_interface.hpp"
#include "logged_user.hpp"
#include "types.hpp"
#include "uread.hpp"
#include "utility.hpp"

namespace lmail
{

class CmdKeyGen final : public ICmd
{
    CmdArgs args_;
    std::shared_ptr<LoggedUser> logged_user_;

public:
    explicit CmdKeyGen(CmdArgs args, std::shared_ptr<LoggedUser> logged_user)
        : args_(std::move(args))
        , logged_user_(std::move(logged_user))
    {
        if (!logged_user_)
            throw std::invalid_argument("logged user provided cannot be empty");
    }

    void exec() override
    try {
        namespace fs = std::filesystem;

        auto keyname = keyname_t{args_.front().value_or(keyname_t{})};
        if (keyname.empty() && !uread(keyname, "Enter a new key name: "))
            return;

        if (keyname.empty()) {
            std::cerr << "key name cannot be empty\n";
            return;
        }

        keyname = fs::path(keyname).filename();
        if (keyname.empty()) {
            std::cerr << "key name provided cannot be empty\n";
            return;
        }

        std::cout << "Trying to add key '" << keyname << "' key...\n";

        auto keys_pair_dir = logged_user_->profile().keys_dir() / keyname;
        keys_pair_dir += Application::kUserKeyLinkSuffix;
        if (fs::exists(keys_pair_dir)) {
            std::cerr << "key with name '" << keyname << "' already exists\n"
                      << "Select another name for a new key\n";
            return;
        }

        size_t key_size = Application::kDefaultRSAKeySize;
        std::string key_size_str;
        // clang-format off
        std::string const prompt = "Enter a new RSA key's size (minimum: "
            + std::to_string(Application::kMinRSAKeyLen)
            + ", default: " + std::to_string(key_size) + "): ";
        // clang-format on
        if (!uread(key_size_str, prompt))
            return;
        if (!key_size_str.empty()) {
            if (auto const key_size_tmp = boost::lexical_cast<size_t>(key_size_str); key_size_tmp >= Application::kMinRSAKeyLen) {
                key_size = key_size_tmp;
            } else {
                std::cerr << "key size cannot be less than " << Application::kMinRSAKeyLen << '\n';
                return;
            }
        } else {
            std::cout << "key size is unspecified. Used default size " << key_size << '\n';
        }
        std::cout << "generation key '" << keyname << "', key size " << key_size << ". Wait a while ...";
        std::cout.flush();
        Application::instance().store(generate_rsa_key_pair(key_size), keys_pair_dir);
        std::cout << "\nsuccessfully generated key '" << keyname << "', key size " << key_size << std::endl;
    } catch (boost::bad_lexical_cast const&) {
        std::cerr << "error: key size expected as a positive integer number\n";
    } catch (std::exception const& ex) {
        std::cerr << "error occurred: " << ex.what() << '\n';
    } catch (...) {
        std::cerr << "unknown exception\n";
    }
};

} // namespace lmail
