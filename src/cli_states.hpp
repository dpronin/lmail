#pragma once

#include <memory>
#include <stdexcept>

#include "fsm.hpp"
#include "state.hpp"
#include "storage.hpp"
#include "types.hpp"
#include "user.hpp"
#include "inbox.hpp"

namespace lmail
{

class CliState;
using CliFsm = Fsm<CliState>;

class CliState : public State
{
public:
    ~CliState() override = default;

protected:
    explicit CliState(CliFsm &fsm) : fsm_(std::addressof(fsm)) {}

public:
    cmds_t commands();

    virtual prompt_t prompt();
    virtual void process(args_t args);

protected:
    virtual std::string default_colored(std::string const &input);
    virtual help_cmds_t const& help_cmds();

protected:
    CliFsm  *fsm_;
};

class InitState : public CliState
{
public:
    ~InitState() override = default;
    explicit InitState(CliFsm &fsm);
};

class MainState : public CliState
{
public:
    explicit MainState(CliFsm &fsm, std::shared_ptr<Storage> storage);
    ~MainState() override = default;

    prompt_t prompt() override;
    void process(args_t args) override;

protected:
    std::string default_colored(std::string const &input) override;
    help_cmds_t const& help_cmds() override;

protected:
    std::shared_ptr<Storage> storage_;
};

class LoggedInState : public MainState
{
public:
    explicit LoggedInState(CliFsm &fsm, std::shared_ptr<Storage> storage, User user, std::shared_ptr<Inbox> inbox);
    ~LoggedInState() override = default;

public:
    void OnEnter() override;
    void OnExit() override;

    prompt_t prompt() override;
    void process(args_t args) override;

protected:
    help_cmds_t const& help_cmds() override;

private:
    username_t login_name();

private:
    User                   user_;
    std::shared_ptr<Inbox> inbox_;
};

} // namespace lmail
