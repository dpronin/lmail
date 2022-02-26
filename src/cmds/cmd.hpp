#pragma once

#include <utility>

#include "sm/cli.hpp"

#include "cmd_args.hpp"
#include "cmd_interface.hpp"
#include "types.hpp"

namespace lmail
{

class Cmd : public ICmd
{
protected:
    sm::Cli& fsm_;
    CmdArgs args_;

    explicit Cmd(sm::Cli& fsm, CmdArgs args = {})
        : fsm_(fsm)
        , args_(std::move(args))
    {
    }

public:
    ~Cmd() override = default;

    Cmd(Cmd const&) = delete;
    Cmd& operator=(Cmd const&) = delete;

    Cmd(Cmd&&)   = delete;
    Cmd& operator=(Cmd&&) = delete;
};

} // namespace lmail
