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
    std::shared_ptr<ICommandLister> lister_;

public:
    static Readline& instance();

    bool operator()(user_input_t& user_input, std::string_view prompt = {});

    void set_cmd_lister(std::shared_ptr<ICommandLister> lister)
    {
        if (!lister)
            throw std::invalid_argument("command lister cannot be empty");
        lister_ = std::move(lister);
    }

    [[nodiscard]] auto cmds() const { return lister_->cmds(); }

private:
    explicit Readline(rl_completion_func_t completer) { rl_attempted_completion_function = completer; }
};

} // namespace lmail
