#pragma once

#include <memory>

#include "fsm.hpp"
#include "logged_user.hpp"
#include "state.hpp"
#include "storage.hpp"
#include "types.hpp"

#include "main_state.hpp"

namespace lmail
{

class LoggedInState : public MainState
{
public:
    explicit LoggedInState(CliFsm &fsm, std::shared_ptr<Storage> storage, std::shared_ptr<LoggedUser> logged_user);
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
