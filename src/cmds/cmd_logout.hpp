#pragma once

#include <memory>
#include <stdexcept>

#include "cmd.hpp"
#include "logged_user.hpp"
#include "storage.hpp"

#include "sm/cli.hpp"

namespace lmail
{

class CmdLogout final : public Cmd
{
    std::shared_ptr<LoggedUser> user_;
    std::shared_ptr<Storage> storage_;

public:
    explicit CmdLogout(sm::Cli& fsm, std::shared_ptr<LoggedUser> user, std::shared_ptr<Storage> storage);
    void exec() override;
};

} // namespace lmail
