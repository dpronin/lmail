#pragma once

#include <memory>

#include "cmd_interface.hpp"
#include "command_lister_interface.hpp"
#include "types.hpp"

namespace lmail
{

class CliState : public ICommandLister
{
public:
    CliState()           = default;
    ~CliState() override = default;

    CliState(CliState const&) = default;
    CliState& operator=(CliState const&) = default;

    CliState(CliState&&) = default;
    CliState& operator=(CliState&&) = default;

    [[nodiscard]] virtual prompt_t prompt() const                  = 0;
    [[nodiscard]] virtual std::unique_ptr<ICmd> parse(args_t args) = 0;
};

} // namespace lmail
