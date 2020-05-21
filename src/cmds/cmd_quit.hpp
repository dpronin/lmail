#pragma once

#include <memory>
#include <stdexcept>
#include <utility>

#include "cli_states.hpp"

namespace lmail
{

class CmdQuit
{
public:
    explicit CmdQuit(CliFsm &cli_fsm) : cli_fsm_(std::addressof(cli_fsm))
    {
        if (!cli_fsm_)
            throw std::invalid_argument("fsm provided cannot be empty");
    }

    void operator()() { cli_fsm_->change_state(std::make_shared<InitState>(*cli_fsm_)); }

private:
    CliFsm *cli_fsm_;
};

} // namespace lmail
