#pragma once

#include "cli_state.hpp"
#include "types.hpp"

namespace lmail
{

class CmdState : public CliState
{
public:
    ~CmdState() override = default;

    cmds_names_t cmds() const override final;
    void process(args_t args) override;
    prompt_t prompt() const override { return ""; }

protected:
    explicit CmdState(cmds_t cmds = {});

private:
    cmds_t cmds_;
};

} // namespace lmail
