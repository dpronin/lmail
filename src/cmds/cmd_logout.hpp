#pragma once

#include <memory>
#include <stdexcept>
#include <utility>

#include "logged_user_utility.hpp"
#include "storage.hpp"
#include "types.hpp"

#include "states/main_state.hpp"

#include "cli_sm.hpp"
#include "cmd_interface.hpp"

namespace lmail
{

class CmdLogout final : public ICmd
{
    sm::Cli& fsm_;
    std::shared_ptr<LoggedUser> user_;
    std::shared_ptr<Storage> storage_;

public:
    explicit CmdLogout(sm::Cli& fsm, std::shared_ptr<LoggedUser> user, std::shared_ptr<Storage> storage)
        : fsm_(fsm)
        , user_(std::move(user))
        , storage_(std::move(storage))
    {
        if (!user_)
            throw std::invalid_argument("user provided cannot be empty");
        if (!storage_)
            throw std::invalid_argument("storage provided cannot be empty");
    }

    void exec() override { fsm_.process_event(sm::ev::logout{std::make_shared<MainState>(fsm_, storage_), make_logged_user_exiter(user_)}); }
};

} // namespace lmail
