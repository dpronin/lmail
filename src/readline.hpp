#pragma once

#include <string>
#include <string_view>

#include "readline/history.h"
#include "readline/readline.h"

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

    void init(rl_completion_func_t completer) { rl_attempted_completion_function = completer; }

    void        set_cmds(cmds_t cmds) { cmds_ = std::move(cmds); }
    auto const &cmds() const noexcept { return cmds_; }

private:
    cmds_t cmds_;
};

} // namespace lmail
