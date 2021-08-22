#pragma once

#include "cli_state.hpp"
#include "types.hpp"

#include "boost/format.hpp"

namespace lmail
{

class CmdState : public CliState
{
public:
    ~CmdState() override = default;

    cmds_names_t cmds() const override final;
    void         process(args_t args) override;

protected:
    explicit CmdState(cmds_t cmds = {});

    virtual std::string default_colored(std::string_view input) const = 0;

private:
    cmds_t cmds_;
};

} // namespace lmail
