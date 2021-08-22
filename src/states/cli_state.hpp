#pragma once

#include "command_lister_interface.hpp"
#include "types.hpp"

namespace lmail
{

class CliState : public ICommandLister
{
public:
    ~CliState() override = default;

    virtual prompt_t prompt() const       = 0;
    virtual void     process(args_t args) = 0;
};

} // namespace lmail
