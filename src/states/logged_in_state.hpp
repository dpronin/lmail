#pragma once

#include <memory>

#include "fsm.hpp"
#include "inbox.hpp"
#include "state.hpp"
#include "storage.hpp"
#include "types.hpp"
#include "user.hpp"

#include "main_state.hpp"

namespace lmail
{

class LoggedInState : public MainState
{
public:
    explicit LoggedInState(CliFsm &fsm, std::shared_ptr<Storage> storage, std::unique_ptr<User> user, std::shared_ptr<Inbox> inbox);
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
    std::shared_ptr<User>  user_;
    std::shared_ptr<Inbox> inbox_;
};

} // namespace lmail
