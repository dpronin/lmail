#pragma once

#include <string_view>
#include <utility>

#include "cli_state.hpp"
#include "types.hpp"

namespace lmail
{

class CmdState final : public CliState
{
    cmds_names_t cmds_names_;
    prompt_t prompt_;

public:
    explicit CmdState(cmds_names_t cmds_names = {}, std::string_view prompt = "")
        : cmds_names_(std::move(cmds_names))
        , prompt_(prompt)
    {
    }
    ~CmdState() override = default;

    CmdState(CmdState const&)            = default;
    CmdState& operator=(CmdState const&) = default;

    CmdState(CmdState&&) noexcept            = default;
    CmdState& operator=(CmdState&&) noexcept = default;

    [[nodiscard]] cmds_names_t cmds() const override { return cmds_names_; }
    [[nodiscard]] prompt_t prompt() const override { return prompt_; }
};

} // namespace lmail
