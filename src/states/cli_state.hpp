#pragma once

#include "cmds/cmd_lister_interface.hpp"

#include "types.hpp"

namespace lmail
{

class CliState : public ICmdLister
{
public:
    CliState()           = default;
    ~CliState() override = default;

    CliState(CliState const&)            = default;
    CliState& operator=(CliState const&) = default;

    CliState(CliState&&) noexcept            = default;
    CliState& operator=(CliState&&) noexcept = default;

    [[nodiscard]] virtual prompt_t prompt() const = 0;
};

} // namespace lmail
