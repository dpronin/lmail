#pragma once

#include <memory>

#include "cli_sm.hpp"
#include "logged_user.hpp"
#include "main_state.hpp"
#include "state.hpp"
#include "storage.hpp"
#include "types.hpp"

namespace lmail
{

class LoggedInState : public MainState
{
public:
    explicit LoggedInState(sm::Cli &fsm, std::shared_ptr<Storage> storage, std::shared_ptr<LoggedUser> logged_user);
    ~LoggedInState() override = default;

public:
    void OnEnter() override;
    void OnExit() override;

    prompt_t prompt() const override;
    void     process(args_t args) override;

protected:
    static help_cmds_t const &help_cmds();

private:
    username_t login_name() const;

private:
    std::shared_ptr<LoggedUser> logged_user_;
};

} // namespace lmail
