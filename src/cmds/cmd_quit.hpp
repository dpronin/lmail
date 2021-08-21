#pragma once

#include <stdexcept>
#include <utility>

#include "cli_sm.hpp"

namespace lmail
{

class CmdQuit final
{
public:
    explicit CmdQuit(sm::Cli &fsm) : fsm_(std::addressof(fsm))
    {
        if (!fsm_)
            throw std::invalid_argument("fsm provided cannot be empty");
    }

    void operator()() { fsm_->process_event(sm::ev::quit{}); }

private:
    sm::Cli *fsm_;
};

} // namespace lmail
