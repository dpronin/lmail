#pragma once

#include <stdexcept>

#include "cmd_state.hpp"

namespace lmail
{

class InitState : public CmdState
{
public:
    InitState()           = default;
    ~InitState() override = default;

    InitState(InitState const&)            = default;
    InitState& operator=(InitState const&) = default;

    InitState(InitState&&) noexcept            = default;
    InitState& operator=(InitState&&) noexcept = default;

    [[nodiscard]] prompt_t prompt() const override { throw std::logic_error("invalid state to receive prompt"); }
};

} // namespace lmail
