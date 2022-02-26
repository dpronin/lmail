#pragma once

#include <memory>

#include "cli_sm.hpp"
#include "logged_user.hpp"
#include "main_state.hpp"
#include "storage.hpp"
#include "types.hpp"

namespace lmail
{

class LoggedInState : public MainState
{
    std::shared_ptr<LoggedUser> user_;

public:
    explicit LoggedInState(sm::Cli& fsm, std::shared_ptr<Storage> storage, std::shared_ptr<LoggedUser> user);
    ~LoggedInState() override = default;

    LoggedInState(LoggedInState const&) = delete;
    LoggedInState& operator=(LoggedInState const&) = delete;

    LoggedInState(LoggedInState&&) = delete;
    LoggedInState& operator=(LoggedInState&&) = delete;

    [[nodiscard]] prompt_t prompt() const override;
};

} // namespace lmail
