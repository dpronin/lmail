#pragma once

#include <memory>
#include <stdexcept>
#include <utility>

#include "storage.hpp"
#include "types.hpp"

#include "states/main_state.hpp"

#include "cli_sm.hpp"

namespace lmail
{

class CmdLogout final
{
public:
    explicit CmdLogout(sm::Cli &fsm, std::shared_ptr<Storage> storage)
        : fsm_(fsm), storage_(std::move(storage))
    {
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void operator()() { fsm_.process_event(sm::ev::logout{std::make_shared<MainState>(fsm_, storage_)}); }

private:
    sm::Cli &                fsm_;
    std::shared_ptr<Storage> storage_;
};

} // namespace lmail
