#pragma once

#include <memory>
#include <stdexcept>
#include <utility>

#include "storage.hpp"
#include "types.hpp"

#include "states/main_state.hpp"

namespace lmail
{

class CmdLogout
{
public:
    explicit CmdLogout(CliFsm &cli_fsm, std::shared_ptr<Storage> storage)
        : cli_fsm_(std::addressof(cli_fsm)), storage_(std::move(storage))
    {
        if (!cli_fsm_)
            throw std::invalid_argument("fsm provided cannot be empty");
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void operator()() { cli_fsm_->change_state(std::make_shared<MainState>(*cli_fsm_, storage_)); }

private:
    CliFsm *                 cli_fsm_;
    std::shared_ptr<Storage> storage_;
};

} // namespace lmail
