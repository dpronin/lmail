#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "readline/readline.h"

#include "cmds/cmd_lister_interface.hpp"

#include "types.hpp"

namespace lmail
{

class Readline
{
    std::shared_ptr<ICmdLister> lister_;

public:
    static Readline& instance();

    user_input_t operator()(std::string_view prompt = "");

    void set_cmd_lister(std::shared_ptr<ICmdLister> lister)
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
