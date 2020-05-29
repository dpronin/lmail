#pragma once

#include "cli_state.hpp"
#include "types.hpp"

#include "cmds/cmd_help.hpp"

namespace lmail
{

class CmdState : public CliState
{
public:
    ~CmdState() override = default;

    cmds_t const &commands() const noexcept override final { return cmds_; }

    prompt_t prompt() const override;
    void     process(args_t args) override;


protected:
    explicit CmdState(help_cmds_t help_cmds = {});

    void                help();
    virtual std::string default_colored(std::string_view input) const;

private:
    cmds_t  cmds_;
    CmdHelp help_;
};

} // namespace lmail
