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
    virtual std::vector<cmd_t> commands();

    explicit CliState(CliFsm &fsm) : fsm_(std::addressof(fsm)) {}

public:
    void OnEnter() override;

    virtual prompt_t prompt();
    virtual void process(args_t args);

protected:
    CliFsm *fsm_;
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

protected:
    prompt_t prompt() override;
    std::vector<cmd_t> commands() override;

public:
    void process(args_t args) override;

private:
    std::shared_ptr<Storage> storage_;
};

class LoggedInState : public CliState
{
public:
    explicit LoggedInState(CliFsm &fsm, User user, std::shared_ptr<Storage> storage, std::shared_ptr<Inbox> inbox);
    ~LoggedInState() override = default;

protected:
    std::vector<cmd_t> commands() override;

public:
    void OnEnter() override;
    void OnExit() override;

    prompt_t prompt() override;
    void process(args_t args) override;

private:
    User                     user_;
    std::shared_ptr<Storage> storage_;
    std::shared_ptr<Inbox>   inbox_;
};

} // namespace lmail
