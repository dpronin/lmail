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
public:
    explicit MainState(sm::Cli &fsm, std::shared_ptr<Storage> storage);
    ~MainState() override = default;

    prompt_t prompt() const override;
    void     process(args_t args) override;

protected:
    static help_cmds_t const &help_cmds();

    explicit MainState(sm::Cli &fsm, std::shared_ptr<Storage> storage, help_cmds_t help_cmds);
    std::string default_colored(std::string_view input) const override;

protected:
    sm::Cli *                fsm_;
    std::shared_ptr<Storage> storage_;
};

} // namespace lmail
