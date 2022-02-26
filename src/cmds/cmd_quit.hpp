#pragma once

#include <utility>

#include "cli_sm.hpp"
#include "cmd_interface.hpp"

namespace lmail
{

class CmdQuit final : public ICmd
{
    sm::Cli& fsm_;

public:
    explicit CmdQuit(sm::Cli& fsm)
        : fsm_(fsm)
    {
    }

    void exec() override { fsm_.process_event(sm::ev::quit{}); }
};

} // namespace lmail
