#pragma once

#include "cli_state.hpp"
#include "types.hpp"

namespace lmail
{

class CmdState : public CliState
{
    cmds_t cmds_;

public:
    ~CmdState() override = default;

    CmdState(CmdState const&) = default;
    CmdState& operator=(CmdState const&) = default;

    CmdState(CmdState&&) = default;
    CmdState& operator=(CmdState&&) = default;

    [[nodiscard]] cmds_names_t cmds() const final;
    [[nodiscard]] std::unique_ptr<ICmd> parse(args_t args) override;
    [[nodiscard]] prompt_t prompt() const override { return ""; }

protected:
    explicit CmdState(cmds_t cmds = {});
};

} // namespace lmail
