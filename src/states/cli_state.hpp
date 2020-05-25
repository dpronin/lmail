#pragma once

#include "state.hpp"
#include "types.hpp"

namespace lmail
{

class CliState : public State
{
public:
    ~CliState() override = default;

    virtual cmds_t const &commands() const noexcept = 0;
    virtual prompt_t      prompt() const            = 0;
    virtual void          process(args_t args)      = 0;
};

} // namespace lmail
