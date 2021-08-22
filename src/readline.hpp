#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

#include "readline/history.h"
#include "readline/readline.h"

#include "command_lister_interface.hpp"
#include "types.hpp"

namespace lmail
{

class Readline
{
public:
    bool operator()(std::string &user_input, std::string_view prompt = {})
    {
        if (auto *input = readline(prompt.data()))
        {
            user_input = input;
            free(input);
            if (!user_input.empty())
                add_history(user_input.c_str());
            return true;
        }
        return false;
    }

    void reset_completer(rl_completion_func_t completer = {}) { rl_attempted_completion_function = completer; }

    void set_cmd_lister(std::shared_ptr<ICommandLister> lister)
    {
        if (!lister)
            throw std::invalid_argument("command lister cannot be empty");
        lister_ = std::move(lister);
    }

    auto cmds() const { return lister_->cmds(); }

private:
    std::shared_ptr<ICommandLister> lister_;
};

} // namespace lmail
