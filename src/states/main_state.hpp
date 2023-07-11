#pragma once

#include <memory>

#include "cli_sm.hpp"
#include "cmd_state.hpp"
#include "storage.hpp"
#include "types.hpp"

namespace lmail
{

class MainState : public CmdState
{
protected:
    sm::Cli& fsm_;

public:
    explicit MainState(sm::Cli& fsm, std::shared_ptr<Storage> storage);
    ~MainState() override = default;

    MainState(MainState const&)            = delete;
    MainState& operator=(MainState const&) = delete;

    MainState(MainState&&)            = delete;
    MainState& operator=(MainState&&) = delete;

    [[nodiscard]] prompt_t prompt() const override;

protected:
    explicit MainState(sm::Cli& fsm, cmds_t cmds);

    [[nodiscard]] std::string default_colored(std::string_view input) const;
};

} // namespace lmail
