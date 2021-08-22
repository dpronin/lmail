#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "readline/readline.h"

#include "command_lister_interface.hpp"
#include "types.hpp"

namespace lmail
{

class Readline
{
private:
    explicit Readline(rl_completion_func_t completer)
    {
        rl_attempted_completion_function = completer;
    }

public:
    static Readline &instance();

    bool operator()(std::string &user_input, std::string_view prompt = {});

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
