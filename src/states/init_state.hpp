#pragma once

#include <stdexcept>

#include "cmd_state.hpp"

namespace lmail
{

class InitState : public CmdState
{
public:
    ~InitState() override = default;

    prompt_t prompt() const override { throw std::logic_error("invalid state to receive prompt"); }
};

} // namespace lmail
