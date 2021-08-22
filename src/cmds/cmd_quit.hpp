#pragma once

#include <utility>

#include "cli_sm.hpp"

namespace lmail
{

class CmdQuit final
{
public:
    explicit CmdQuit(sm::Cli &fsm) : fsm_(fsm)
    {
    }

    void operator()() { fsm_.process_event(sm::ev::quit{}); }

private:
    sm::Cli &fsm_;
};

} // namespace lmail
