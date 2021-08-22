#pragma once

#include "cmd_state.hpp"

#include "color.hpp"

namespace lmail
{

class InitState : public CmdState
{
public:
    ~InitState() override = default;

    prompt_t    prompt() const override { throw std::logic_error("invalid state to receive prompt"); }
    std::string default_colored(std::string_view input) const override { throw std::logic_error("invalid state to receive default color"); }
};

} // namespace lmail
